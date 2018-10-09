#pragma once

#include "Common.h"

#include <vector>
#include <array>

namespace Core
{
	template<typename T, uint32 size>
	class ArrayVector
	{
	private:
		std::array<std::vector<T>, size> _vectorArray;

	public:
		template<uint32 level>
		uint32 PushBack(const T& object)
		{
			static_assert(level < size, "Error, Out of Range!");

			auto& vector = _vectorArray[level];
			vector.push_back(object);
			return vector.size() - 1;
		}

		template<uint32 level>
		void Insert(const T& object, uint32 index)
		{
			static_assert(level < size, "Error, Out of Range!");

			auto& vector = _vectorArray[level];
			vector.insert(vector.begin() + index, object);
		}

		template<uint32 fromLevel, uint32 toLevel>
		void Copy(uint32 fromIndex)
		{
			static_assert(fromLevel < size, "Error, Out of Range!");
			static_assert(toLevel < size, "Error, Out of Range!");

			auto& fromVector	= _vectorArray[fromLevel];
			auto& toVector		= _vectorArray[toLevel];

			toVector.push_back(fromVector[fromIndex]);
		}

		template<uint32 fromLevel, uint32 toLevel>
		void Copy(uint32 fromIndex, uint32 toIndex)
		{
			static_assert(fromLevel < size, "Error, Out of Range!");
			static_assert(toLevel < size, "Error, Out of Range!");

			auto& fromVector	= _vectorArray[fromLevel];
			auto& toVector		= _vectorArray[toLevel];

			toVector.insert(toVector.begin() + toIndex, fromVector[fromIndex]);
		}

		template<uint32 fromLevel, uint32 toLevel>
		void Move(uint32 fromIndex)
		{
			Copy<fromLevel, toLevel>(fromIndex);
			Delete<fromLevel>(fromIndex);
		}

		template<uint32 fromLevel, uint32 toLevel>
		void Move(uint32 fromIndex, uint32 toIndex)
		{
			Copy<fromLevel, toLevel>(fromIndex, toIndex);
			Delete<fromLevel>(fromIndex);
		}

		template<uint32 level>
		void Delete(uint32 index)
		{
			static_assert(level < size, "Error, Out of Range!");

			auto& vector = _vectorArray[level];
			vector.erase(vector.begin() + index);
		}

		template<uint32 level>
		void DeleteAll()
		{
			static_assert(level < size, "Error, Out of Range!");
			_vectorArray[level].clear();
		}

		template<uint32 level>
		const T& Get(uint32 index) const
		{
			static_assert(level < size, "Error, Out of Range!");
			return _vectorArray[level][index];
		}

		template<uint32 level>
		T& Get(uint32 index)
		{
			return const_cast<T&>(static_cast<const ArrayVector<T, size>*>(this)->Get<level>(index));
		}

		template<uint32 level>
		size_t GetSize() const
		{
			static_assert(level < size, "Error, Out of Range!");
			return _vectorArray[level].size();
		}

		static constexpr uint32 GetMaxLelvel() { return size; }
	};
}