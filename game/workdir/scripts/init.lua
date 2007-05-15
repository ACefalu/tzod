-- init.lua
-- autorun script


dofile("scripts/func.lua")       -- define some usefull functions
dofile("scripts/vehicles.lua")   -- register vehicle classes
dofile("scripts/weapons.lua")    -- set up parameters of weapons
dofile("scripts/names.lua")      -- fill the random_names array

--=============================================================================

print("loading startup level");

loadmap("maps/intro.map")


message("����� ���������� � ���� ����: ���� ������")


for _,i in pairs{"red","yellow","blue","FBI Tank","neutral"} do
	addplayer{cls="default", skin=i}
end



--message("-----------------------------------------")
--message("����� ������ ����, ������� F2")
--message("����� ������� � ������� ����, ������� ESC")

--pause(false)
