DROP FUNCTION update_signal_value(character varying, integer);
 
CREATE OR REPLACE FUNCTION update_signal_value(
    IN _signal_name character varying,
    IN _value integer)
  RETURNS void AS

$BODY$
BEGIN

  EXECUTE 'update signals set '        ||
		'brand_name = '''     || _brand_name     || ''', ' ||
		'description = '''    || _description     || ''''  ||
	'where id = ' || _id;


END;
$BODY$
LANGUAGE plpgsql VOLATILE
COST 100;
ALTER FUNCTION update_brand(character varying,
			     text,
			     integer)
  OWNER TO postgres;