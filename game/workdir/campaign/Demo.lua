-- Demo

conf.sv_nightmode = true
loadmap("campaign/Demo/maps/part1.map")


local time = 2

print("test\n");

pushcmd(function() message("������!") end, time)
time = time + 1;


pushcmd(function() message("��� ��������� ������������ ������������ ����������� �����") end, time)
time = time + 2;


pushcmd(function() message("������ ����� ��������� �����") end, time)
time = time + 2;

pushcmd(function() conf.sv_nightmode = false; loadmap("campaign/Demo/maps/part1.map") end, time)
time = time + 1;

pushcmd(function() message("��� ��� :)") end, time)
time = time + 2;


pushcmd(function() message("������������� �������� ��������� ���:") end, time)
time = time + 1;


pushcmd(function() message("- ��������� �������") end, time)
time = time + 1;


pushcmd(function()
  for i = 1, 10 do
    pushcmd(function() actor("wall_brick", 256 + i*32, 128 + i*32) end, 0.1 * i)
  end
  for i = 1, 10 do
    pushcmd(function() actor("wall_brick", 384 + i*32, 128 + i*32) end, 0.1 * i)
  end
end, time)
time = time + 1;



