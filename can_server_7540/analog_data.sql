SELECT --i_s.signal_name, i_s.signal_offset, 
	s.signal_name, s.data_offset, s.signal_index,
	a_d.*
  FROM analog_data a_d
  --JOIN input_signals i_s ON a_d.can_id = i_s.can_id --AND a_d.dev_index * 32 = i_s.signal_offset
  JOIN signals s ON s.cob_id = a_d.can_id
  ORDER BY 1, 2

  update signals set data_offset = 32 where data_offset = 33
  
