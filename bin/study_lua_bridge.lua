--
--test Namespaces
--
--print(test,test.detail,test.utility)


--
--test Data, Properties, Functions, and CFunctions
--
--[[
test.var1 = 5         -- okay
--test.var2 = 6         -- error: var2 is not writable
--test.prop1 = "Hello"  -- okay
--test.prop1 = 68       -- okay, Lua converts the number to a string.
--test.prop2 = "bar"    -- error: prop2 is not writable
test.foo()           -- calls foo and discards the return value
test.var1 = test.foo ()    -- calls foo and stores the result in var1
test.bar ("Employee") -- calls bar with a string
--test.bar (test)       -- error: bar expects a string not a table
--]]


--
--test Class Objects
--
local AClassObj = test.A ()  --create class A instance

print("before:",test.A.staticData) -- access class A static member 
test.A.staticData = 8  -- modify class A static member
print("after:",test.A.staticData)

print("before:", test.A.getStaticProperty())
--test.A.staticProperty = 1.2 --error:can not modify

print("staticCFunc")
test.A.staticCFunc() --Call the static method

AClassObj.data = "sting"
print("dataMember:",AClassObj.data)

AClassObj.prop = 'a'
print("property:",AClassObj.prop)

AClassObj:func1() --Notice: NO AClassObj.func1() 
--test.A.func1(AClassObj) -- Equivalent to AClassObj:func1()

AClassObj:virtualFunc()

AClassObj:cfunc("lua_State*",42)

BClassObj = test.B("constructor register",42) --First Call base class A constructor function 

BClassObj:func1()

BClassObj:func2()

BClassObj:virtualFunc()  --virtualFunc In Class B

print("dataMember1",a.data1)
a.data1 = 32
print("a dataMember1",a.data1)

print("pointer ac dataMember1",ac.data1)
--ac.data1 = 32   --error: because ac is registed as const

print("pointer ac2 dataMember1",ac.data1)
--ac2.data1 = 32  --error: because ac2 is registed as const

--ap.data1 = 42
--print("ap dataMember1",ap.data1)

a = nil; collectgarbage ()      -- 'a' still exists in C++.
b = nil; collectgarbage ()      -- Lua calls ~B() on the copy of b.