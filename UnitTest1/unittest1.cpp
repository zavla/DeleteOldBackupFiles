#include "stdafx.h"
#include "CppUnitTest.h"

#include "../DeleteOldFilesCxx11/row2.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

template<> 
static
std::wstring 
	Microsoft::VisualStudio::CppUnitTestFramework::
ToString<my::LoadLineRetvalue> (const my::LoadLineRetvalue & r){
	return std::to_wstring(static_cast<int>(r));
}

namespace UnitTest1
{		
	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(TestMethod_LoadLine)
		{
			// TODO: Your test code here
			my::row			one_row;
			std::string		line1{ "//comment" };
			Assert::AreEqual (one_row.LoadLine(line1),	my::LoadLineRetvalue::ignore);
			//Assert::AreEqual(0, 0);

		}

	};
}