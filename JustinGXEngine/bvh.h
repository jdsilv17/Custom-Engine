
#include <cstdint>
#include <cassert>
#include <DirectXMath.h>

//aabb_t{ float3 min; float3 max; };

namespace end
{
	struct aabb_bounds_t { DirectX::XMFLOAT3 min; DirectX::XMFLOAT3 max; };
	// bvh_node_t
	//
	// BVH will be stored as a vector/array of this type
	// A node will always be at a higher index/address than its parent
	class bvh_node_t 
	{
	public:
		// TODO: This constructor is the only function for you to implement in this file.
		bvh_node_t(bvh_node_t* root, uint32_t left_index, uint32_t right_index)
		{
			*this = *root;
			this->_left = left_index;
			this->_right = right_index;
		}

		bvh_node_t(const aabb_bounds_t& bounds, uint32_t id) : _aabb{ bounds }, _id{ id } {}

		bvh_node_t() = default;
		bvh_node_t(const bvh_node_t&) = default;

		// Root must be at [0], so parent index must be invalid
		inline bool is_root()const { return _parent == UINT32_MAX; }

		inline bool is_branch()const { return _is_branch; }

		inline bool is_leaf()const { return !is_branch(); }
		
		inline uint32_t get_element_id()const 
		{
			assert(is_leaf());
			return _id; 
		}

		inline uint32_t& get_left()
		{
			assert(is_branch());
			return _left;
		}

		inline uint32_t get_left()const
		{
			assert(is_branch());
			return _left;
		}

		inline uint32_t& get_right()
		{
			assert(is_branch());
			return _right;
		}

		inline uint32_t get_right()const
		{
			assert(is_branch());
			return _right;
		}

		inline uint32_t get_parent()const
		{
			assert(!is_root());
			return _parent;
		}

		uint32_t& get_parent()
		{
			//assert(!is_root());
			return _parent;
		}

		aabb_bounds_t& get_aabb()
		{
			return _aabb;
		}

		const aabb_bounds_t& get_aabb()const
		{
			return _aabb;
		}

	private:

		// Indices for connected nodes
		uint32_t _parent = UINT32_MAX;

		// Index for left child.
		// Branches must have two children.
		// If "this" is a leaf, _left/_is_branch will be 0.
		union
		{
			uint32_t _left;
			uint32_t _is_branch;
		};

		// Index for right child.
		// Branches must have two children.
		// If "this" is a leaf, _right/_id stores the element id.
		union
		{
			uint32_t _right;
			uint32_t _id;
		};

		// AABB for this node
		aabb_bounds_t _aabb;
	};
}