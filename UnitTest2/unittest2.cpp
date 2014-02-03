#include "stdafx.h"
#include "CppUnitTest.h"
#include "../rendering_service/rpublicinterface.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest2
{		
	TEST_CLASS(UnitTest2)
	{
	public:
		TEST_METHOD(LoadFbxScene1)
		{
            Assert::AreNotEqual(RHANDLE_INVALID, r_load_fbx_scene("testresources\\test.fbx"));
            Assert::AreEqual(1, r_get_object_count(RSCENEROOTTYPE));
            Assert::AreEqual(1, r_get_object_count(RMESHDATATYPE));
            Assert::AreEqual(1, r_get_object_count(RSIMPLEMESHTYPE));
		}
	};
}