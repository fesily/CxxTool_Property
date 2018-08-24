#ifdef _MSC_VER
#define GTEST_HAS_TR1_TUPLE 0
#endif
#include "gtest/gtest.h"
#include "../include/CxxTool_Property/PropertyComponent.hpp"

TEST(PropertyComponent,TestProperty)
{
	//std::declval<Property<std::string*>>()->append("1");
}

TEST(PropertyComponent, TestScalarReference)
{
	PropertyComponent component;
	PropertyName<int> key{ "keydd123" };
	int val=1;
	component.AddReferenceProperty(key, val);
	auto property = component[key];
	ASSERT_EQ(component[key], 1);
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, 1);
	ASSERT_EQ(property, val);
	bool changed = false;
	component.BindFunctionProertyEvent(key, [&]
	{
		changed = true;
	});
	int val1 = -1;
	component.BindFunctionProertyEvent(key,[&](int v)
	{
		val1 = v;
	});
	component[key] = 2;
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], 2);
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, 2);
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, val);

	changed = false;
	component[key] += 1;
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], 3);
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, 3);
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, val);

	changed = false;
	val1 = component[key] + 1;
	ASSERT_FALSE(changed);
	ASSERT_EQ(component[key], 3);
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, 3);
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, 4);
}

TEST(PropertyComponent,TestReference)
{
	PropertyComponent component;
	PropertyName<std::string> key{ "key21321" };
	std::string val = "1";
	component.AddReferenceProperty(key, val);
	auto property = component[key];
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, val);
	ASSERT_EQ(property, val);
	bool changed = false;
	component.BindFunctionProertyEvent(key, [&]
	{
		changed = true;
	});
	std::string val1;
	component.BindFunctionProertyEvent(key, [&](const std::string& v)
	{
		val1 = v;
	});
	component.BindFunctionProertyEvent(key, [&](std::string v)
	{
		val1 = std::move(v);
	});
	component[key] = "2";
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], "2");
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, "2");
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, val);

	changed = false;
	component[key]->append("3");
	component[key].OnPropertyChanged();
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], "23");
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, "23");
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, val);


	changed = false;
	val1 = component[key] + "4";
	ASSERT_FALSE(changed);
	ASSERT_EQ(component[key], "23");
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, "23");
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, "234");

}

TEST(PropertyComponent, TestScalarValueByLeftReference)
{
	PropertyComponent component;
	PropertyName<int> key{ "keydd123" };
	int val = 1;
	component.AddValueProperty(key, val);
	auto property = component[key];
	ASSERT_EQ(component[key], 1);
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, 1);
	ASSERT_EQ(property, val);
	bool changed = false;
	component.BindFunctionProertyEvent(key, [&]
	{
		changed = true;
	});
	int val1 = -1;
	component.BindFunctionProertyEvent(key, [&](int v)
	{
		val1 = v;
	});
	component[key] = 2;
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], 2);
	ASSERT_NE(component[key], val);
	ASSERT_EQ(property, 2);
	ASSERT_NE(property, val);
	ASSERT_NE(val1, val);
	ASSERT_EQ(val1, 2);

	changed = false;
	component[key] += 1;
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], 3);
	ASSERT_NE(component[key], val);
	ASSERT_EQ(property, 3);
	ASSERT_NE(property, val);
	ASSERT_NE(val1, val);
	ASSERT_EQ(val1, 3);

	changed = false;
	val1 = component[key] + 1;
	ASSERT_FALSE(changed);
	ASSERT_EQ(component[key], 3);
	ASSERT_NE(component[key], val);
	ASSERT_EQ(property, 3);
	ASSERT_NE(property, val);
	ASSERT_EQ(val1, 4);
}

void TestScalarValue(PropertyComponent& component,Property<int>& property,const PropertyName<int>& key)
{
	
	bool changed = false;
	component.BindFunctionProertyEvent(key, [&]
	{
		changed = true;
	});
	int val1 = -1;
	component.BindFunctionProertyEvent(key, [&](int v)
	{
		val1 = v;
	});
	component[key] = 2;
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], 2);
	ASSERT_EQ(property, 2);
	ASSERT_EQ(val1, 2);

	changed = false;
	component[key] += 1;
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], 3);
	ASSERT_EQ(property, 3);
	ASSERT_EQ(val1, 3);

	changed = false;
	val1 = component[key] + 1;
	ASSERT_FALSE(changed);
	ASSERT_EQ(component[key], 3);
	ASSERT_EQ(property, 3);
	ASSERT_EQ(val1, 4);
}

TEST(PropertyComponent, TestScalarValueByRightReference)
{
	PropertyComponent component;
	PropertyName<int> key{ "keydd123" };
	component.AddValueProperty(key, 1);
	auto property = component[key];
	ASSERT_EQ(component[key], 1);
	ASSERT_EQ(property, 1);
	TestScalarValue(component, property, key);
}

TEST(PropertyComponent, TestScalarValueByEmplaceValue)
{
	PropertyComponent component;
	PropertyName<int> key{ "keydd123" };
	component.EmplaceValueProperty(key, 1);
	auto property = component[key];
	ASSERT_EQ(component[key], 1);
	ASSERT_EQ(property, 1);
	TestScalarValue(component, property, key);
}

TEST(PropertyComponent, TestScalarValueByEmplaceEmpty)
{
	PropertyComponent component;
	PropertyName<int> key{ "keydd123" };
	component.EmplaceValueProperty(key);
	auto property = component[key];
	ASSERT_EQ(component[key], int{});
	ASSERT_EQ(property, int{});
	TestScalarValue(component, property, key);
}

TEST(PropertyComponent, TestValueByLeftReference)
{
	PropertyComponent component;
	PropertyName<std::string> key{ "key21321" };
	std::string val = "1";
	component.AddValueProperty(key, val);
	auto property = component[key];
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, val);
	ASSERT_EQ(property, val);
	bool changed = false;
	component.BindFunctionProertyEvent(key, [&]
	{
		changed = true;
	});
	std::string val1;
	component.BindFunctionProertyEvent(key, [&](const std::string& v)
	{
		val1 = v;
	});
	component.BindFunctionProertyEvent(key, [&](std::string v)
	{
		val1 = std::move(v);
	});
	component[key] = "2";
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], "2");
	ASSERT_NE(component[key], val);
	ASSERT_EQ(property, "2");
	ASSERT_NE(property, val);
	ASSERT_NE(val1, val);

	changed = false;
	component[key]->append("3");
	component[key].OnPropertyChanged();
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], "23");
	ASSERT_NE(component[key], val);
	ASSERT_EQ(property, "23");
	ASSERT_NE(property, val);
	ASSERT_NE(val1, val);


	changed = false;
	val1 = component[key] + "4";
	ASSERT_FALSE(changed);
	ASSERT_EQ(component[key], "23");
	ASSERT_NE(component[key], val);
	ASSERT_EQ(property, "23");
	ASSERT_NE(property, val);
	ASSERT_EQ(val1, "234");
}

TEST(PropertyComponent, TestValueByRightReference)
{
	PropertyComponent component;
	PropertyName<std::string> key{ "key21321" };
	component.AddValueProperty(key, static_cast<std::string>("1"));
	auto property = component[key];
	ASSERT_EQ(component[key], "1");
	ASSERT_EQ(property, "1");
	bool changed = false;
	component.BindFunctionProertyEvent(key, [&]
	{
		changed = true;
	});
	std::string val1;
	component.BindFunctionProertyEvent(key, [&](const std::string& v)
	{
		val1 = v;
	});
	component.BindFunctionProertyEvent(key, [&](std::string v)
	{
		val1 = std::move(v);
	});
	component[key] = "2";
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], "2");
	ASSERT_EQ(property, "2");

	changed = false;
	component[key]->append("3");
	component[key].OnPropertyChanged();
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], "23");
	ASSERT_EQ(property, "23");


	changed = false;
	val1 = component[key] + "4";
	ASSERT_FALSE(changed);
	ASSERT_EQ(component[key], "23");
	ASSERT_EQ(property, "23");
	ASSERT_EQ(val1, "234");
}

TEST(PropertyComponent, TestValueByEmplaceValue)
{
	PropertyComponent component;
	PropertyName<std::string> key{ "key21321" };
	component.EmplaceValueProperty(key, "1");
	auto property = component[key];
	ASSERT_EQ(component[key], "1");
	ASSERT_EQ(property, "1");
	bool changed = false;
	component.BindFunctionProertyEvent(key, [&]
	{
		changed = true;
	});
	std::string val1;
	component.BindFunctionProertyEvent(key, [&](const std::string& v)
	{
		val1 = v;
	});
	component.BindFunctionProertyEvent(key, [&](std::string v)
	{
		val1 = std::move(v);
	});
	component[key] = "2";
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], "2");
	ASSERT_EQ(property, "2");

	changed = false;
	component[key]->append("3");
	component[key].OnPropertyChanged();
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], "23");
	ASSERT_EQ(property, "23");


	changed = false;
	val1 = component[key] + "4";
	ASSERT_FALSE(changed);
	ASSERT_EQ(component[key], "23");
	ASSERT_EQ(property, "23");
	ASSERT_EQ(val1, "234");
}

TEST(PropertyComponent, TestValueByEmplaceEmpty)
{
	PropertyComponent component;
	PropertyName<std::string> key{ "key21321" };
	component.EmplaceValueProperty(key);
	auto property = component[key];
	ASSERT_EQ(component[key], "");
	ASSERT_EQ(property, "");
	bool changed = false;
	component.BindFunctionProertyEvent(key, [&]
	{
		changed = true;
	});
	std::string val1;
	component.BindFunctionProertyEvent(key, [&](const std::string& v)
	{
		val1 = v;
	});
	component.BindFunctionProertyEvent(key, [&](std::string v)
	{
		val1 = std::move(v);
	});
	component[key] = "2";
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], "2");
	ASSERT_EQ(property, "2");

	changed = false;
	component[key]->append("3");
	component[key].OnPropertyChanged();
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], "23");
	ASSERT_EQ(property, "23");


	changed = false;
	val1 = component[key] + "4";
	ASSERT_FALSE(changed);
	ASSERT_EQ(component[key], "23");
	ASSERT_EQ(property, "23");
	ASSERT_EQ(val1, "234");
}

TEST(PropertyComponent, TestScalarFunction_get)
{
	PropertyComponent component;
	PropertyName<int> key{ "keydd123" };
	int val = 1;
	component.AddFunctionProperty(key, [&]() {return val; });
	auto property = component[key];
	ASSERT_EQ(component[key], 1);
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, 1);
	ASSERT_EQ(property, val);
	val = 0;
	ASSERT_EQ(component[key], 0);
	ASSERT_EQ(property, 0);

	bool changed = false;
	component.BindFunctionProertyEvent(key, [&]
	{
		changed = true;
	});
	int val1 = -1;
	component.BindFunctionProertyEvent(key, [&](int v)
	{
		val1 = v;
	});
	ASSERT_THROW(component[key] = 2, PropertyBadAccess);
	ASSERT_FALSE(changed);
	ASSERT_NE(component[key], 2);
	ASSERT_EQ(component[key], val);
	ASSERT_NE(property, 2);
	ASSERT_EQ(property, val);
	ASSERT_NE(val1, val);

	changed = false;
	ASSERT_THROW(component[key] += 1, PropertyBadAccess);
	ASSERT_FALSE(changed);
	ASSERT_NE(component[key], 3);
	ASSERT_EQ(component[key], val);
	ASSERT_NE(property, 3);
	ASSERT_EQ(property, val);
	ASSERT_NE(val1, val);

	changed = false;
	val1 = component[key] + 1;
	ASSERT_FALSE(changed);
	ASSERT_EQ(component[key], 0);
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, 0);
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, 1);
}

TEST(PropertyComponent, TestScalarFunction_get_set)
{
	PropertyComponent component;
	PropertyName<int> key{ "keydd123" };
	int val = 1;
	component.AddFunctionProperty(key, [&]() {return val; }, [&](int v) {val = v; });
	auto property = component[key];
	ASSERT_EQ(component[key], 1);
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, 1);
	ASSERT_EQ(property, val);
	val = 0;
	ASSERT_EQ(component[key], 0);
	ASSERT_EQ(property, 0);

	bool changed = false;
	component.BindFunctionProertyEvent(key, [&]
	{
		changed = true;
	});
	int val1 = -1;
	component.BindFunctionProertyEvent(key, [&](int v)
	{
		val1 = v;
	});
	component[key] = 2;
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], 2);
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, 2);
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, val);

	changed = false;
	component[key] += 1;
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], 3);
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, 3);
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, val);

	changed = false;
	val1 = component[key] + 1;
	ASSERT_FALSE(changed);
	ASSERT_EQ(component[key], 3);
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, 3);
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, 4);
}

TEST(PropertyComponent, TestFunction_get)
{
	PropertyComponent component;
	PropertyName<std::string> key{ "key21321" };
	std::string val = "1";
	component.AddFunctionProperty(key, [&]()->std::string& {return val; });
	auto property = component[key];
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(component[key], "1");
	ASSERT_EQ(property, val);
	ASSERT_EQ(property, "1");
	val = "0";
	ASSERT_EQ(component[key], "0");
	ASSERT_EQ(property, "0");
	bool changed = false;
	component.BindFunctionProertyEvent(key, [&]
	{
		changed = true;
	});
	std::string val1;
	component.BindFunctionProertyEvent(key, [&](const std::string& v)
	{
		val1 = v;
	});
	component.BindFunctionProertyEvent(key, [&](std::string v)
	{
		val1 = std::move(v);
	});
	ASSERT_THROW(component[key] = "2", PropertyBadAccess);
	ASSERT_FALSE(changed);
	ASSERT_NE(component[key], "2");
	ASSERT_EQ(component[key], val);
	ASSERT_NE(property, "2");
	ASSERT_EQ(property, val);
	ASSERT_NE(val1, val);

	changed = false;
	component[key]->append("3");
	component[key].OnPropertyChanged();
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], "03");
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, "03");
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, val);


	changed = false;
	val1 = component[key] + "4";
	ASSERT_FALSE(changed);
	ASSERT_EQ(component[key], "03");
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, "03");
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, "034");
}

TEST(PropertyComponent, TestFunction_get_set)
{
	PropertyComponent component;
	PropertyName<std::string> key{ "key21321" };
	std::string val = "1";
	component.AddFunctionProperty(key, [&]()->std::string& {return val; }, [&](const std::string& v)
	{
		val = v;
	});
	auto property = component[key];
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(component[key], "1");
	ASSERT_EQ(property, val);
	ASSERT_EQ(property, "1");
	val = "0";
	ASSERT_EQ(component[key], "0");
	ASSERT_EQ(property, "0");
	bool changed = false;
	component.BindFunctionProertyEvent(key, [&]
	{
		changed = true;
	});
	std::string val1;
	component.BindFunctionProertyEvent(key, [&](const std::string& v)
	{
		val1 = v;
	});
	component.BindFunctionProertyEvent(key, [&](std::string v)
	{
		val1 = std::move(v);
	});
	component[key] = "2";
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], "2");
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, "2");
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, val);

	changed = false;
	component[key]->append("3");
	component[key].OnPropertyChanged();
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], "23");
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, "23");
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, val);


	changed = false;
	val1 = component[key] + "4";
	ASSERT_FALSE(changed);
	ASSERT_EQ(component[key], "23");
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, "23");
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, "234");
}

TEST(PropertyComponent, TestFunction_get_set_move)
{
	PropertyComponent component;
	PropertyName<std::string> key{ "key21321" };
	std::string val = "1";
	component.AddFunctionProperty(key, [&]()->std::string& {return val; }, [&](const std::string& v)
	{
		val = v;
	},[&](std::string&& v)
	{
		val = std::move(v);
	});
	auto property = component[key];
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(component[key], "1");
	ASSERT_EQ(property, val);
	ASSERT_EQ(property, "1");
	val = "0";
	ASSERT_EQ(component[key], "0");
	ASSERT_EQ(property, "0");
	bool changed = false;
	component.BindFunctionProertyEvent(key, [&]
	{
		changed = true;
	});
	std::string val1;
	component.BindFunctionProertyEvent(key, [&](const std::string& v)
	{
		val1 = v;
	});
	component.BindFunctionProertyEvent(key, [&](std::string v)
	{
		val1 = std::move(v);
	});
	component[key] = "2";
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], "2");
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, "2");
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, val);

	changed = false;
	component[key]->append("3");
	component[key].OnPropertyChanged();
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], "23");
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, "23");
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, val);


	changed = false;
	val1 = component[key] + "4";
	ASSERT_FALSE(changed);
	ASSERT_EQ(component[key], "23");
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, "23");
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, "234");

	std::string mov("123");
	component[key] = std::move(mov);
	ASSERT_TRUE(mov.empty());
	ASSERT_TRUE(changed);
	ASSERT_EQ(component[key], "123");
	ASSERT_EQ(component[key], val);
	ASSERT_EQ(property, "123");
	ASSERT_EQ(property, val);
	ASSERT_EQ(val1, val);
}