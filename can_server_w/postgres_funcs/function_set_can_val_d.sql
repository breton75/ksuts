-- Function: set_can_val_d(integer, bigint)

-- DROP FUNCTION set_can_val_d(integer, bigint);

CREATE OR REPLACE FUNCTION set_can_val_d(
    _can_id integer,
    _value bigint)
  RETURNS void AS
$BODY$
  declare
	signals_1 character varying;
	signals_0 character varying;

BEGIN

  EXECUTE 'UPDATE digital_data SET '
			'dev_data = '     || _value  || ' ' ||
			'WHERE can_id = ' || _can_id;
			

  IF (_value IS NULL) | (_value < 0) THEN
  
	EXECUTE 'UPDATE signals_data SET value = -1.0 ' ||
		'WHERE signal_index IN (' ||
			'SELECT signal_index FROM signals WHERE cob_id = ' || can_id || ')';

  ELSE

	FOR i IN 0..63 LOOP
	
		IF (_value >> i) & 1 = 1 THEN
			signals_1 = signals_1 || i || ',';
		ELSE
			signals_0 = signals_0 || i || ',';
		END IF;

	END LOOP;

	signals_1 = left(signals_1, length(signals_1) - 1);
	signals_0 = left(signals_0, length(signals_0) - 1);
	
	--trim(trailing ',' from signals_1);
	--trim(trailing ',' from signals_0);

	IF signals_1 <> '' THEN
	
		EXECUTE 'UPDATE signals_data SET value = 1.0, last_update = now() ' ||
			'WHERE signal_index IN (' ||
				'SELECT signal_index FROM signals WHERE cob_id = ' || can_id || ' AND ' ||
				'data_offset IN (' || signals_1 || '))';
	END IF;

	IF signals_0 <> '' THEN

		EXECUTE 'UPDATE signals_data SET value = 0.0, last_update = now() ' ||
			'WHERE signal_index IN (' ||
				'SELECT signal_index FROM signals WHERE cob_id = ' || can_id || ' AND ' ||
				'data_offset IN (' || signals_0 || '))';
	END IF;

  END IF;


END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION set_can_val_d(integer, bigint)
  OWNER TO postgres;

MAC LABEL ON FUNCTION set_can_val_d(integer, bigint) IS '{0,0}';
