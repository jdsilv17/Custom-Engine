#include "WindowContainer.h"

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

LRESULT CALLBACK HandleMsgRedirect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // All other messages
    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;

    default:
    {
        // retrieve ptr to window class
        WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        // forward message to window class handler
        return pWindow->WndProc(hWnd, message, wParam, lParam);
    }
    }
}

//
//  FUNCTION: HandleMsgSetup(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_NCCREATE:
    {
        // retrieve the pointer to windowContainer that we pass in at Init
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        WindowContainer* pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);
        if (pWindow == nullptr)
        {
            //ErrorLogger::Log("Critical Error: Pointer to window container is null during WM_NCCREATE.");
            exit(-1);
        }
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
        return pWindow->WndProc(hWnd, message, wParam, lParam);
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM WindowContainer::MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = HandleMsgSetup;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_JUSTINGXENGINE));
    wcex.hCursor = LoadCursor(nullptr, IDC_CROSS);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_JUSTINGXENGINE);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL WindowContainer::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    // find resize stuff
    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, this); // this is passed in as the 

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT WindowContainer::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //HDC hdc;
    RECT rcClient;                 // client area rectangle 
    POINT ptClientUL;              // client upper left corner 
    POINT ptClientLR;              // client lower right corner 

    //static POINTS mouseCoords;        // beginning point
    /****************** Make these members of WindowContainer ********************/
    static POINT prev_point = { 0,0 };
    static POINT curr_point = { 0,0 };
    static POINT delta_point = { 0,0 };

    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            //DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_KEYDOWN:
    {
        if (wParam == VK_UP) // move gizmo forward
        {
            this->GFX.bits.set(0);
        }
        if (wParam == VK_DOWN) // move gizmo backward
        {
            this->GFX.bits.set(1);
        }
        if (wParam == VK_LEFT) // rotate gizmo left
        {
            this->GFX.bits.set(2);
        }
        if (wParam == VK_RIGHT) // rotate gizmo right
        {
            this->GFX.bits.set(3);
        }
        if (wParam == 'W') // move camera forward
        {
            this->GFX.bits.set(4);
        }
        if (wParam == 'A') // strafe camera left
        {
            this->GFX.bits.set(5);
        }
        if (wParam == 'S') // move camera backward
        {
            this->GFX.bits.set(6);
        }
        if (wParam == 'D') // strafe camera right
        {
            this->GFX.bits.set(7);
        }
        if (wParam == VK_SPACE) // move camera up
        {
            this->GFX.bits.set(8);
        }
        if (wParam == 'X') // move camera down
        {
            this->GFX.bits.set(9);
        }
        if (wParam == VK_OEM_COMMA) // Step Back through animation
        {
            this->GFX.run_anim.FrameStepBack();
            //this->GFX.bits.set(12);
        }
        if (wParam == VK_OEM_PERIOD) // Step Forward through animation
        {
            this->GFX.run_anim.FrameStepForward();
            //this->GFX.bits.set(13);
        }
        if (wParam == 'R') // reset Animation to frame 0
        {
            this->GFX.bits.set(14);
        }
        if (wParam == 'P') // start/stop animation playback
        {
            if (!this->GFX.run_anim.IsPlaying())
                this->GFX.run_anim.StartPlayback();
            else
                this->GFX.run_anim.StopPlayback();
        }
        break;
    }
    case WM_KEYUP:
    {
        if (wParam == VK_UP)
        {
            this->GFX.bits.set(0, false);
        }
        if (wParam == VK_DOWN)
        {
            this->GFX.bits.set(1, false);
        }
        if (wParam == VK_LEFT)
        {
            this->GFX.bits.set(2, false);
        }
        if (wParam == VK_RIGHT)
        {
            this->GFX.bits.set(3, false);
        }
        if (wParam == 'W')
        {
            this->GFX.bits.set(4, false);
        }
        if (wParam == 'A')
        {
            this->GFX.bits.set(5, false);
        }
        if (wParam == 'S')
        {
            this->GFX.bits.set(6, false);
        }
        if (wParam == 'D')
        {
            this->GFX.bits.set(7, false);
        }
        if (wParam == VK_SPACE)
        {
            this->GFX.bits.set(8, false);
        }
        if (wParam == 'X')
        {
            this->GFX.bits.set(9, false);
        }
        if (wParam == VK_OEM_COMMA)
        {
            this->GFX.bits.set(12, false);
        }
        if (wParam == VK_OEM_PERIOD)
        {
            this->GFX.bits.set(13, false);
        }
        if (wParam == 'R')
        {
            this->GFX.bits.set(14, false);
        }
        break;
    }
    case WM_RBUTTONDOWN:
    {
        // Capture mouse input. 

        SetCapture(hWnd);

        // Retrieve the screen coordinates of the client area, 
        // and convert them into client coordinates. 

        GetClientRect(hWnd, &rcClient);
        ptClientUL.x = rcClient.left;
        ptClientUL.y = rcClient.top;

        // Add one to the right and bottom sides, because the 
        // coordinates retrieved by GetClientRect do not 
        // include the far left and lowermost pixels. 

        ptClientLR.x = rcClient.right + 1;
        ptClientLR.y = rcClient.bottom + 1;
        ClientToScreen(hWnd, &ptClientUL);
        ClientToScreen(hWnd, &ptClientLR);

        // Copy the client coordinates of the client area 
        // to the rcClient structure. Confine the mouse cursor 
        // to the client area by passing the rcClient structure 
        // to the ClipCursor function. 

        SetRect(&rcClient, ptClientUL.x, ptClientUL.y,
            ptClientLR.x, ptClientLR.y);
        ClipCursor(&rcClient); // confines the cursor within the client area

        // Convert the cursor coordinates into a POINTS 
        // structure, which defines the beginning point of the 
        // line drawn during a WM_MOUSEMOVE message. 

        curr_point.x = LOWORD(lParam);
        curr_point.y = HIWORD(lParam);
        return 0;
    }
    ////case WM_INPUT:
    ////    {
    ////        UINT dataSize;
    ////        GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));
    ////        if (dataSize > 0)
    ////        {
    ////            std::unique_ptr<BYTE[]> rawData = std::make_unique<BYTE[]>(dataSize);
    ////            if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawData.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
    ////            {
    ////                RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawData.get());
    ////                if (raw->header.dwType == RIM_TYPEMOUSE) // check if raw data is a mouse data type
    ////                {
    ////                    mouseCoords.x = raw->data.mouse.lLastX;
    ////                    mouseCoords.y = raw->data.mouse.lLastY;
    ////                }
    ////            }
    ////        }
    ////
    ////
    ////        return DefWindowProc(hWnd, message, wParam, lParam);
    ////    }
    case WM_MOUSEMOVE:
    {
        //When moving the mouse, the user must hold down 
        //the right mouse button to rotate the camera. 
        if (wParam & MK_RBUTTON)
        {
            prev_point = curr_point;
            curr_point.x = LOWORD(lParam);
            curr_point.y = HIWORD(lParam);

            // calc delta of mouse pos
            delta_point.x = curr_point.x - prev_point.x;
            delta_point.y = curr_point.y - prev_point.y;
            prev_point = curr_point;
            this->GFX.cam.UpdateRotation(static_cast<float>(delta_point.y) * 0.005f, static_cast<float>(delta_point.x) * 0.005f, 0.0f);
        }

        break;
    }
    case WM_RBUTTONUP:
        ClipCursor(NULL);
        ReleaseCapture();
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
