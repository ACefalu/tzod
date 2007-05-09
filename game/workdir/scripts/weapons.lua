-- weapons description
-- written by Insert
-------------------------------------------------------------------------------

-- helper functions
local function adjust_health(veh, h)
  veh.health = veh.health * (h / classes.default.health)
end

-- ���� �����. ��� ������� ������ ���������� ��������������� �������, �������
-- ����� ������ ����� ��������� ������.

-------------------------------------------------------------------------------

function gc.weap_rockets.attach(veh)
--  message("����������� ������: ���������")
  adjust_health(veh, 85)
end

-------------------------------------------------------------------------------

function gc.weap_autocannon.attach(veh)
--  message("����������� ������: �������������� �����")
  adjust_health(veh, 80)
end

-------------------------------------------------------------------------------

function gc.weap_cannon.attach(veh)
--  message("����������� ������: ������� �����")
  adjust_health(veh, 125)
end

-------------------------------------------------------------------------------

function gc.weap_plazma.attach(veh)
--  message("����������� ������: ���������� �����")
  adjust_health(veh, 100)
end

-------------------------------------------------------------------------------

function gc.weap_gauss.attach(veh)
--  message("����������� ������: ����� ������")
  adjust_health(veh, 70)
end

-------------------------------------------------------------------------------

function gc.weap_ram.attach(veh)
--  message("����������� ������: �����")
--  veh.mass = veh.mass * 2                -- �������, ����� =)
  adjust_health(veh, 350)                -- � ������
  veh.percussion  = veh.percussion * 8
end

-------------------------------------------------------------------------------

function gc.weap_bfg.attach(veh)
--  message("����������� ������: ���");
  adjust_health(veh, 110)
end

-------------------------------------------------------------------------------

function gc.weap_ripper.attach(veh)
--  message("����������� ������: �����")
  adjust_health(veh, 80)
end

-------------------------------------------------------------------------------

function gc.weap_minigun.attach(veh)
--  message("����������� ������: �������")
  veh.mass = veh.mass * 0.8
  adjust_health(veh, 65)
end

-------------------------------------------------------------------------------
-- end of file
