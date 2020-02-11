update devices set device_name = 'Пожарная автоматика СС1-119'   where device_index = 20;
update devices set device_name = 'Пожарная автоматика СС1-122'   where device_index = 21;
update devices set device_name = 'Пожарная автоматика 12СС1-218' where device_index = 22;
update devices set device_name = 'Пожарная автоматика 12СС1-8'   where device_index = 23;
update devices set device_name = 'Пожарная автоматика 12СС1-38'  where device_index = 24;
update devices set device_name = 'Пожарная автоматика 12СС1-67'  where device_index = 25;
update devices set device_name = 'Пожарная автоматика 12СС1-93'  where device_index = 26;
update devices set device_name = 'Пожарная автоматика 12СС1-123' where device_index = 27;
update devices set device_name = 'Пожарная автоматика 12СС1-150' where device_index = 28;

delete from devices where device_index not in (40, 20,21,22,23,24,25,26,27,28,39,83, 99);

update signals set device_index = 20, timeout = 0  where device_index in (20,21,22,23,24,25,26,27,28,29,30,82);