CREATE SEQUENCE public.ktss_id_seq
  INCREMENT 1
  MINVALUE 1
  MAXVALUE 9223372036854775807
  START 1
  CACHE 1;
ALTER TABLE public.ktss_id_seq
  OWNER TO postgres;

CREATE TABLE ktss (
    id                INTEGER NOT NULL DEFAULT nextval('ktss_id_seq'::regclass),
    kts_name        VARCHAR (100),
    ifc_id          INTEGER,
    protocol_id     INTEGER,
    data_type       BOOLEAN        DEFAULT (True),
    data_length     INTEGER,
    driver_lib_path VARCHAR (1024),
    description     TEXT
);


