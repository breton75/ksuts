DROP FUNCTION update_signal_value(integer, real);
 
CREATE OR REPLACE FUNCTION update_signal_value(
    IN _signal_id integer,
    IN _value real)
  RETURNS void AS

$BODY$
BEGIN

  EXECUTE 'update signals_data set '	||
		'value = '	|| _value	|| ', ' ||
		'last_update = now() '		||
	'where signal_id = ' || _signal_id || '';


END;
$BODY$
LANGUAGE plpgsql VOLATILE
COST 100;
ALTER FUNCTION update_signal_value(integer, real)
  OWNER TO postgres;