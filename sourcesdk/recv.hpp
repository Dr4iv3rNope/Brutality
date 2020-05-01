#pragma once
#include "sdk.hpp"

namespace SourceSDK
{
	struct RecvTable;

	struct RecvProp final
	{
		enum class Type : int
		{
			Int,
			Float,
			Vector,
			String,
			Array,
			DataTable,
			NumSendPropTypes
		};

		const char* name;
		Type type;
		int flags;
		int stringLength;
		bool isInsideArray;
		const void* extraData;

		// if array

		const RecvProp* propArray; // array of props
		const void* propArrayProxy;

		const void* proxy;

		// if data table

		const void* tableProxy;
		const RecvTable* table; // pointer to table

		int offset;

		int elementStride;
		int elementCount;

		// can be null
		const char* parentArrayName;
	};

	struct RecvTable final
	{
		const RecvProp* propArray;
		int propCount;

	private:
		const void* decoder;

	public:
		const char* netTableName;

		bool isInitalized;
		bool inMainList;
	};
}
