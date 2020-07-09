-- Function: update_analog_data(smallint, smallint, integer, bytea)

-- DROP FUNCTION update_analog_data(smallint, smallint, integer, bytea);

CREATE OR REPLACE FUNCTION update_analog_data(
    v_port_id smallint,
    v_pack_size smallint,
    v_can_id integer,
    v_can_value bytea)
  RETURNS void AS
$BODY$
DECLARE
	dev_counter smallint;
	str_offset smallint;
	max_idx smallint;
	sig_val real;
BEGIN
IF v_pack_size = 4 THEN max_idx = 0;
ELSIF v_pack_size = 8 THEN max_idx = 1;
ELSE max_idx = -1;
END IF;
dev_counter := 0;
WHILE dev_counter <= max_idx
LOOP
	str_offset := dev_counter * 4 + 1;
	sig_val = bytea_to_float(SUBSTRING(v_can_value FROM str_offset FOR 4));
	UPDATE analog_data
	SET dev_data = sig_val --bytea_to_float(SUBSTRING(v_can_value FROM str_offset FOR 4))
	WHERE can_id = v_can_id
	AND dev_index = dev_counter;

	UPDATE signals_data SET value = sig_val WHERE signal_index IN (
			SELECT signal_index 
			FROM signals 
			WHERE cob_id = v_can_id 
			AND data_offset  = dev_counter * 32);
	
	dev_counter := dev_counter + 1;
END LOOP;

END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION update_analog_data(smallint, smallint, integer, bytea)
  OWNER TO postgres;

MAC LABEL ON FUNCTION update_analog_data(smallint, smallint, integer, bytea) IS '{0,0}';
