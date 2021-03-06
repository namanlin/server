# Tests for PERFORMANCE_SCHEMA
# Make sure mysql_upgrade does not destroy data in a 'performance_schema'
# database.
# 

# Some initial settings + Preemptive cleanup
let $MYSQLD_DATADIR= `SELECT @@datadir`;
let $err_file= $MYSQLTEST_VARDIR/tmp/pfs_upgrade.err;
let $out_file= $MYSQLTEST_VARDIR/tmp/pfs_upgrade.out;
--error 0,1
--remove_file $out_file
--error 0,1
--remove_file $err_file

--disable_warnings
drop table if exists test.user_table;
drop procedure if exists test.user_proc;
drop function if exists test.user_func;
drop event if exists test.user_event;
--enable_warnings


--echo "Testing mysql_upgrade with TABLE performance_schema.user_table"

create table test.user_table(a int);

--error 0,1
--remove_file $MYSQLD_DATADIR/performance_schema/user_table.frm
--copy_file $MYSQLD_DATADIR/test/user_table.frm $MYSQLD_DATADIR/performance_schema/user_table.frm

# Make sure the table is visible
use performance_schema;
show tables like "user_table";

--source suite/perfschema/include/upgrade_check.inc

# Make sure the table is still visible
show tables like "user_table";

use test;

--remove_file $MYSQLD_DATADIR/performance_schema/user_table.frm
drop table test.user_table;


--echo "Testing mysql_upgrade with VIEW performance_schema.user_view"

create view test.user_view as select "Not supposed to be here";

--error 0,1
--remove_file $MYSQLD_DATADIR/performance_schema/user_view.frm
--copy_file $MYSQLD_DATADIR/test/user_view.frm $MYSQLD_DATADIR/performance_schema/user_view.frm

# Make sure the view is visible
use performance_schema;
show tables like "user_view";

--source suite/perfschema/include/upgrade_check.inc

# Make sure the view is still visible
show tables like "user_view";

use test;

--remove_file $MYSQLD_DATADIR/performance_schema/user_view.frm
drop view test.user_view;


--echo "Testing mysql_upgrade with PROCEDURE performance_schema.user_proc"

create procedure test.user_proc()
  select "Not supposed to be here";

update mysql.proc set db='performance_schema' where name='user_proc';

--source suite/perfschema/include/upgrade_check.inc

select name from mysql.proc where db='performance_schema';

update mysql.proc set db='test' where name='user_proc';
drop procedure test.user_proc;


--echo "Testing mysql_upgrade with FUNCTION performance_schema.user_func"

create function test.user_func() returns integer
  return 0;

update mysql.proc set db='performance_schema' where name='user_func';

--source suite/perfschema/include/upgrade_check.inc

select name from mysql.proc where db='performance_schema';

update mysql.proc set db='test' where name='user_func';
drop function test.user_func;


--echo "Testing mysql_upgrade with EVENT performance_schema.user_event"

create event test.user_event on schedule every 1 day do
  select "not supposed to be here";

update mysql.event set db='performance_schema' where name='user_event';

--source suite/perfschema/include/upgrade_check.inc

select name from mysql.event where db='performance_schema';

update mysql.event set db='test' where name='user_event';
drop event test.user_event;

