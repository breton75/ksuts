--DROP FUNCTION get_soeg(int, int);

CREATE OR REPLACE FUNCTION get_soeg(
	IN _start_register int,
	IN _register_count int)
	
  RETURNS TABLE(register_data int) AS

$BODY$
  declare
	signals_r record;
	b bytea;

begin
  
  FOR i IN _start_register.._start_register + _register_count - 1
  LOOP
  
	register_data = 0;
	
	FOR signals_r IN EXECUTE 'SELECT signals_data.value::int as val, signals.soeg_offset as offset, signals.soeg_data_length as len, signals.soeg_data_type as typ ' ||
				 'FROM signals ' ||
				 'LEFT JOIN signals_data ON signals.signal_index = signals_data.signal_index ' ||
				 'WHERE signals.soeg_register = ' || i || ' ORDER BY signals.soeg_offset ASC'
	LOOP

		IF signals_r.typ = 'b' THEN
		
			IF signals_r.val = 1.0 THEN 
			
				register_data = register_data | (1 << signals_r.offset);
				
			END IF;
			
		ELSIF signals_r.typ = 'w' THEN

			IF signals_r.val > 0.0 THEN 
			
				register_data = signals_r.val::integer;

			END IF;

		ELSIF signals_r.typ = 'r' THEN

			IF signals_r.val > 0.0 THEN 
			
				b = float4send(signals_r.val);

				register_data = (get_byte(b, 0) * x'01000000'::int + get_byte(b, 1) * x'010000'::int) >> 16;

				RETURN NEXT;

				register_data = get_byte(b, 2) * x'0100'::int + get_byte(b, 3);
				
			END IF;
			
		ELSIF signals_r.typ = 'u' THEN

			IF signals_r.val > 0.0 THEN 
			
				register_data = signals_r.val::int >> 16;

				RETURN NEXT;

				register_data = signals_r.val::int & x'0000FFFF'::int;
				
			END IF;

		END IF;

		--iCounter = iCounter + 1;
	
	END LOOP;

	RETURN NEXT;

  END LOOP;

  -- ! обязательно возвращаем стиль обратно
  --set datestyle to ISO; 

end;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION get_soeg(int, int)
  OWNER TO postgres;



