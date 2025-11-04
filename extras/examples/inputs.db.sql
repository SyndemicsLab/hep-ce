BEGIN TRANSACTION;
DROP TABLE IF EXISTS "population";
CREATE TABLE "population" (
	"id"	INTEGER NOT NULL UNIQUE,
	"sex"	INTEGER NOT NULL DEFAULT 0,  -- male
	"age"	INTEGER NOT NULL DEFAULT 0, -- in months
	"is_alive"	INTEGER NOT NULL DEFAULT 1 CHECK (is_alive IN (0, 1)), -- default dead
	"boomer_classification"	INTEGER NOT NULL DEFAULT 0 CHECK (boomer_classification IN (0, 1)), -- default not boomer
	"death_reason"	INTEGER NOT NULL DEFAULT -1, -- not dead
	"drug_behavior"	INTEGER NOT NULL DEFAULT 0, -- never used
	"time_last_active_drug_use"	INTEGER NOT NULL DEFAULT 0,
	"hcv"	INTEGER NOT NULL DEFAULT 0, -- no hcv
	"fibrosis_state"	INTEGER NOT NULL DEFAULT -1, -- no fibrosis
	"is_genotype_three"	INTEGER NOT NULL DEFAULT 0 CHECK (is_genotype_three IN (0, 1)), -- not genotype 3
	"seropositive"	INTEGER NOT NULL DEFAULT 0 CHECK (seropositive IN (0, 1)), -- not seropositive
	"time_hcv_changed"	INTEGER NOT NULL DEFAULT -1,
	"time_fibrosis_state_changed"	INTEGER NOT NULL DEFAULT -1,
	"times_hcv_infected"	INTEGER NOT NULL DEFAULT 0,
	"times_acute_cleared"	INTEGER NOT NULL DEFAULT 0,
	"svrs"	INTEGER NOT NULL DEFAULT 0,
	"hiv"	TEXT DEFAULT NULL, -- make foreign key
	"time_hiv_changed"	INTEGER DEFAULT NULL,
	"low_cd4_months_count"	INTEGER DEFAULT NULL,
	"hcc_state"	TEXT DEFAULT NULL, -- make foreign key
	"hcc_diagnosed"	INTEGER NOT NULL DEFAULT 0 CHECK (hcc_diagnosed IN (0, 1)),
	"currently_overdosing"	INTEGER NOT NULL DEFAULT 0 CHECK (currently_overdosing IN (0, 1)), -- not overdosing
	"num_overdoses"	INTEGER NOT NULL DEFAULT 0,
	"moud_state"	INTEGER NOT NULL DEFAULT 0, -- not in moud
	"time_started_moud"	INTEGER NOT NULL DEFAULT -1,
	"current_moud_state_concurrent_months"	INTEGER NOT NULL DEFAULT 0,
	"total_moud_months"	INTEGER NOT NULL DEFAULT 0,
	"pregnancy_state"	INTEGER NOT NULL DEFAULT -1, -- NA
	"time_of_pregnancy_change"	INTEGER NOT NULL DEFAULT -1,
	"pregnancy_count"	INTEGER NOT NULL DEFAULT 0,
	"num_infants"	INTEGER NOT NULL DEFAULT 0,
	"num_stillbirths"	INTEGER NOT NULL DEFAULT 0,
	"num_infant_hcv_exposures"	INTEGER NOT NULL DEFAULT 0,
	"num_infant_hcv_infections"	INTEGER NOT NULL DEFAULT 0,
	"num_infant_hcv_tests"	INTEGER NOT NULL DEFAULT 0,
	"measured_fibrosis_state"	INTEGER NOT NULL DEFAULT 4, -- no staging
	"had_second_test"	INTEGER NOT NULL DEFAULT 0 CHECK (had_second_test IN (0, 1)), -- no second test
	"time_of_last_staging"	INTEGER NOT NULL DEFAULT -1,
	"hcv_link_state"	INTEGER NOT NULL DEFAULT 0, -- never linked
	"time_of_hcv_link_change"	INTEGER NOT NULL DEFAULT -1,
	"hcv_link_count"	INTEGER NOT NULL DEFAULT 0,
	"hiv_link_state"	TEXT DEFAULT NULL, -- make foreign key
	"time_of_hiv_link_change"	INTEGER DEFAULT NULL,
	"hiv_link_count"	INTEGER DEFAULT NULL,
	"time_of_last_hcv_screening"	INTEGER NOT NULL DEFAULT -1,
	"num_hcv_ab_tests"	INTEGER NOT NULL DEFAULT 0,
	"num_hcv_rna_tests"	INTEGER NOT NULL DEFAULT 0,
	"hcv_antibody_positive"	INTEGER NOT NULL DEFAULT 0 CHECK (hcv_antibody_positive IN (0, 1)), -- not AB positive
	"hcv_identified"	INTEGER NOT NULL DEFAULT 0 CHECK (hcv_identified IN (0, 1)), -- not currently identified
	"time_hcv_identified"	INTEGER NOT NULL DEFAULT -1,
	"num_hcv_identifications"	INTEGER NOT NULL DEFAULT 0,
	"hcv_screening_type"	TEXT NOT NULL, -- make foreign key
	"num_hcv_false_negatives"	INTEGER NOT NULL DEFAULT 0,
	"identifications_cleared"	INTEGER NOT NULL DEFAULT 0,
	"time_of_last_hiv_screening"	INTEGER DEFAULT NULL,
	"num_hiv_ab_tests"	INTEGER DEFAULT NULL,
	"num_hiv_rna_tests"	TEXT DEFAULT NULL,
	"hiv_antibody_positive"	TEXT DEFAULT NULL,
	"hiv_identified"	INTEGER DEFAULT NULL,
	"time_hiv_identified"	INTEGER  DEFAULT NULL,
	"num_hiv_identified"	INTEGER DEFAULT NULL,
	"hiv_screening_type"	TEXT DEFAULT NULL,
	"initiated_hcv_treatment"	INTEGER NOT NULL DEFAULT 0 CHECK (initiated_hcv_treatment IN (0, 1)), -- not started hcv treatment
	"time_of_hcv_treatment_initiation"	INTEGER NOT NULL DEFAULT -1,
	"num_hcv_treatment_starts"	INTEGER NOT NULL DEFAULT 0,
	"num_hcv_treatment_withdrawals"	INTEGER NOT NULL DEFAULT 0,
	"num_hcv_treatment_toxic_reactions"	INTEGER NOT NULL DEFAULT 0,
	"num_completed_hcv_treatments"	INTEGER NOT NULL DEFAULT 0,
	"num_hcv_salvages"	INTEGER NOT NULL DEFAULT 0,
	"in_hcv_salvage_treatment"	INTEGER NOT NULL DEFAULT 0 CHECK (in_hcv_salvage_treatment IN (0, 1)), -- not in hcv salvage treatment
	"initiated_hiv_treatment"	INTEGER DEFAULT NULL,
	"time_of_hiv_treatment_initiation"	INTEGER DEFAULT NULL,
	"num_hiv_treatment_starts"	INTEGER DEFAULT NULL,
	"num_hiv_treatment_withdrawals"	INTEGER DEFAULT NULL,
	"num_hiv_treatment_toxic_reactions"	REAL DEFAULT NULL,
	"behavior_utility"	REAL NOT NULL DEFAULT 1.0,
	"liver_utility"	REAL NOT NULL DEFAULT 1.0,
	"treatment_utility"	REAL NOT NULL DEFAULT 1.0,
	"background_utility"	REAL NOT NULL DEFAULT 1.0,
	"moud_utility"	REAL NOT NULL DEFAULT 1.0,
	"overdose_utility"	REAL NOT NULL DEFAULT 1.0,
	"hiv_utility"	REAL NOT NULL DEFAULT 1.0,
	"min_utility"	REAL NOT NULL DEFAULT 0.0,
	"mult_utility"	REAL NOT NULL DEFAULT 0.0,
	"discounted_min_utility"	REAL NOT NULL DEFAULT 0.0,
	"discounted_mult_utility"	REAL NOT NULL DEFAULT 0.0,
	"life_span"	REAL NOT NULL DEFAULT 0.0,
	"discounted_life_span"	REAL NOT NULL DEFAULT 0.0,
	"cost"	REAL NOT NULL,
	"discount_cost" REAL NOT NULL DEFAULT 0.0,
    PRIMARY KEY("id"),
    FOREIGN KEY("sex") REFERENCES "sex"("id"),
    FOREIGN KEY("death_reasons") REFERENCES "death_reasons"("id"),
    FOREIGN KEY("drug_behavior") REFERENCES "drug_behaviors"("id"),
    FOREIGN KEY("hcv") REFERENCES "hcv_states"("id"),
    FOREIGN KEY("fibrosis_state") REFERENCES "fibrosis_states"("id"),
    FOREIGN KEY("moud_state") REFERENCES "moud"("id"),
    FOREIGN KEY("pregnancy_state") REFERENCES "pregnancy_states"("id"),
    FOREIGN KEY("measured_fibrosis_state") REFERENCES "fibrosis_diagnosis_states"("id"),
    FOREIGN KEY("hcv_link_state") REFERENCES "link_states"("id"),
    FOREIGN KEY("link_states") REFERENCES "link_states"("id")
);
DROP TABLE IF EXISTS "death_reasons";
CREATE TABLE "death_reasons" (
    "id" INTEGER NOT NULL UNIQUE,
    "reason" TEXT NOT NULL UNIQUE,
    PRIMARY KEY ("id");
);
DROP TABLE IF EXISTS "link_states";
CREATE TABLE "link_states" (
    "id" INTEGER NOT NULL UNIQUE,
    "state" TEXT NOT NULL UNIQUE,
    PRIMARY KEY ("id");
);
DROP TABLE IF EXISTS "antibody_testing";
CREATE TABLE "antibody_testing" (
	"age_years"	INTEGER NOT NULL,
	"drug_behavior"	INTEGER NOT NULL,
	"accept_probability"	REAL NOT NULL,
	PRIMARY KEY("age_years","drug_behavior"),
	FOREIGN KEY("drug_behavior") REFERENCES "drug_behaviors"("id")
);
DROP TABLE IF EXISTS "background_impacts";
CREATE TABLE "background_impacts" (
	"age_years"	INTEGER NOT NULL,
	"gender"	INTEGER NOT NULL,
	"drug_behavior"	INTEGER NOT NULL,
	"utility"	REAL NOT NULL,
	"cost"	REAL NOT NULL DEFAULT 0.0,
	PRIMARY KEY("age_years","gender","drug_behavior"),
	FOREIGN KEY("drug_behavior") REFERENCES "drug_behaviors"("id"),
	FOREIGN KEY("gender") REFERENCES "sex"("id")
);
DROP TABLE IF EXISTS "background_mortality";
CREATE TABLE "background_mortality" (
	"age_years"	INTEGER NOT NULL,
	"gender"	INTEGER NOT NULL,
	"background_mortality"	REAL NOT NULL,
	PRIMARY KEY("age_years","gender"),
	FOREIGN KEY("gender") REFERENCES "sex"("id")
);
DROP TABLE IF EXISTS "behavior_impacts";
CREATE TABLE "behavior_impacts" (
	"gender"	INTEGER NOT NULL,
	"drug_behavior"	INTEGER NOT NULL,
	"cost"	REAL NOT NULL DEFAULT 0.0,
	"utility"	INTEGER NOT NULL,
	PRIMARY KEY("gender","drug_behavior"),
	FOREIGN KEY("drug_behavior") REFERENCES "drug_behaviors"("id"),
	FOREIGN KEY("gender") REFERENCES "sex"("id")
);
DROP TABLE IF EXISTS "behavior_transitions";
CREATE TABLE "behavior_transitions" (
	"age_years"	INTEGER NOT NULL,
	"gender"	INTEGER NOT NULL,
	"drug_behavior"	INTEGER NOT NULL,
	"moud"	INTEGER NOT NULL,
	"never"	REAL NOT NULL DEFAULT 0.0,
	"former_noninjection"	REAL NOT NULL DEFAULT 0.0,
	"former_injection"	REAL NOT NULL DEFAULT 0.0,
	"noninjection"	REAL NOT NULL DEFAULT 0.0,
	"injection"	REAL NOT NULL DEFAULT 0.0,
	PRIMARY KEY("age_years","gender","drug_behavior","moud"),
	FOREIGN KEY("drug_behavior") REFERENCES "drug_behaviors"("id"),
	FOREIGN KEY("gender") REFERENCES "sex"("id"),
	FOREIGN KEY("moud") REFERENCES "moud"("id")
);
DROP TABLE IF EXISTS "bool_lookup";
CREATE TABLE "bool_lookup" (
	"id"	INTEGER NOT NULL UNIQUE,
	"value"	TEXT NOT NULL UNIQUE,
	PRIMARY KEY("id")
);
DROP TABLE IF EXISTS "drug_behaviors";
CREATE TABLE "drug_behaviors" (
	"id"	INTEGER NOT NULL UNIQUE,
	"behavior"	TEXT NOT NULL UNIQUE,
	PRIMARY KEY("id")
);
DROP TABLE IF EXISTS "fibrosis";
CREATE TABLE "fibrosis" (
	"fibrosis_state"	INTEGER NOT NULL,
	"diagnosed_fibrosis"	INTEGER NOT NULL,
	"apri"	REAL NOT NULL,
	"fibroscan"	REAL NOT NULL,
	"fibrotest"	REAL NOT NULL,
	"idealtest"	REAL NOT NULL,
	"fib4"	REAL NOT NULL,
	PRIMARY KEY("fibrosis_state","diagnosed_fibrosis"),
	FOREIGN KEY("diagnosed_fibrosis") REFERENCES "fibrosis_diagnosis_states"("id"),
	FOREIGN KEY("fibrosis_state") REFERENCES "fibrosis_states"("id")
);
DROP TABLE IF EXISTS "fibrosis_diagnosis_states";
CREATE TABLE "fibrosis_diagnosis_states" (
	"id"	INTEGER NOT NULL UNIQUE,
	"state"	TEXT NOT NULL,
	PRIMARY KEY("id")
);
DROP TABLE IF EXISTS "fibrosis_states";
CREATE TABLE "fibrosis_states" (
	"id"	INTEGER NOT NULL UNIQUE,
	"state"	TEXT NOT NULL,
	PRIMARY KEY("id")
);
DROP TABLE IF EXISTS "hcv_impacts";
CREATE TABLE "hcv_impacts" (
	"hcv_status"	INTEGER NOT NULL DEFAULT 0,
	"fibrosis_state"	INTEGER NOT NULL,
	"cost"	REAL NOT NULL DEFAULT 0.0,
	"utility"	REAL NOT NULL,
	PRIMARY KEY("hcv_status","fibrosis_state"),
	FOREIGN KEY("fibrosis_state") REFERENCES "fibrosis_states"("id"),
	FOREIGN KEY("hcv_status") REFERENCES "bool_lookup"("id")
);
DROP TABLE IF EXISTS "hcv_states";
CREATE TABLE "hcv_states" (
	"id"	INTEGER NOT NULL UNIQUE,
	"hcv"	TEXT NOT NULL,
	PRIMARY KEY("id" AUTOINCREMENT)
);
DROP TABLE IF EXISTS "incidence";
CREATE TABLE "incidence" (
	"age_years"	INTEGER NOT NULL,
	"gender"	INTEGER NOT NULL,
	"drug_behavior"	INTEGER NOT NULL,
	"incidence"	REAL NOT NULL,
	PRIMARY KEY("age_years","gender","drug_behavior"),
	FOREIGN KEY("drug_behavior") REFERENCES "drug_behaviors"("id"),
	FOREIGN KEY("gender") REFERENCES "sex"("id")
);
DROP TABLE IF EXISTS "init_cohort";
CREATE TABLE "init_cohort" (
	"id"	TEXT,
	"age_months"	INT,
	"gender"	INT,
	"drug_behavior"	TEXT,
	"time_last_active_drug_use"	INT,
	"seropositivity"	INT,
	"genotype_three"	INT,
	"fibrosis_state"	INT,
	"identified_as_hcv_positive"	INT,
	"link_state"	INT,
	"hcv_status"	INT
);
DROP TABLE IF EXISTS "link_states";
CREATE TABLE "link_states" (
	"id"	INTEGER NOT NULL UNIQUE,
	"value"	TEXT NOT NULL,
	PRIMARY KEY("id")
);
DROP TABLE IF EXISTS "lost_to_follow_up";
CREATE TABLE "lost_to_follow_up" (
	"pregnancy_state"	INTEGER NOT NULL UNIQUE,
	"probability"	REAL NOT NULL,
	PRIMARY KEY("pregnancy_state"),
	FOREIGN KEY("pregnancy_state") REFERENCES "pregnancy_states"("id")
);
DROP TABLE IF EXISTS "moud";
CREATE TABLE "moud" (
	"id"	INTEGER NOT NULL UNIQUE,
	"value"	TEXT NOT NULL,
	PRIMARY KEY("id")
);
DROP TABLE IF EXISTS "pregnancy";
CREATE TABLE "pregnancy" (
	"age_years"	INTEGER NOT NULL UNIQUE,
	"pregnancy_probability"	REAL NOT NULL,
	"stillbirth"	REAL,
	PRIMARY KEY("age_years")
);
DROP TABLE IF EXISTS "pregnancy_states";
CREATE TABLE "pregnancy_states" (
	"id"	INTEGER NOT NULL UNIQUE,
	"state"	TEXT NOT NULL UNIQUE,
	PRIMARY KEY("id" AUTOINCREMENT)
);
DROP TABLE IF EXISTS "screening_and_linkage";
CREATE TABLE "screening_and_linkage" (
	"age_years"	INT,
	"gender"	INT,
	"drug_behavior"	INT,
	"pregnancy"	INT,
	"background_screen_probability"	REAL,
	"background_link_probability"	REAL,
	"intervention_screen_probability"	REAL,
	"intervention_link_probability"	REAL
);
DROP TABLE IF EXISTS "sex";
CREATE TABLE "sex" (
	"id"	INTEGER NOT NULL UNIQUE,
	"sex"	TEXT NOT NULL UNIQUE,
	PRIMARY KEY("id")
);
DROP TABLE IF EXISTS "smr";
CREATE TABLE "smr" (
	"gender"	INTEGER NOT NULL,
	"drug_behavior"	INTEGER NOT NULL,
	"smr"	REAL NOT NULL,
	PRIMARY KEY("gender","drug_behavior"),
	FOREIGN KEY("drug_behavior") REFERENCES "drug_behaviors"("id"),
	FOREIGN KEY("gender") REFERENCES "sex"("id")
);
DROP TABLE IF EXISTS "treatment_initiations";
CREATE TABLE "treatment_initiations" (
	"pregnancy_state"	INTEGER NOT NULL UNIQUE,
	"treatment_initiation"	REAL NOT NULL,
	PRIMARY KEY("pregnancy_state"),
	FOREIGN KEY("pregnancy_state") REFERENCES "pregnancy_states"("id")
);
DROP TABLE IF EXISTS "treatments";
CREATE TABLE "treatments" (
	"salvage"	INT,
	"genotype_three"	INT,
	"cirrhotic"	INT,
	"course"	NUMERIC,
	"duration"	INT,
	"cost"	REAL,
	"svr_prob_if_completed"	REAL,
	"toxicity_prob_if_withdrawal"	REAL,
	"withdrawal"	REAL,
	"toxicity_prob"	REAL
);
CREATE TABLE IF NOT EXISTS "moud_transitions" (
  "age_years" INTEGER NOT NULL,
  "current_moud" INTEGER NOT NULL,
  "current_duration" INTEGER NOT NULL,
  "pregnancy" INTEGER NOT NULL,
  "none" REAL NOT NULL DEFAULT 0.0,
  "current" REAL NOT NULL DEFAULT 0.0,
  "post" REAL NOT NULL DEFAULT 0.0,
  PRIMARY KEY("age_years", "current_moud", "current_duration", "pregnancy"),
  FOREIGN KEY("current_moud") REFERENCES "moud"("id"),
  FOREIGN KEY("pregnancy") REFERENCES "pregnancy_states"("id")
);
CREATE TABLE IF NOT EXISTS "moud_costs" (
  "moud" INTEGER NOT NULL,
  "pregnancy" INTEGER NOT NULL,
  "cost" REAL NOT NULL DEFAULT 0.0,
  "utility" REAL NOT NULL DEFAULT 1.0,
  PRIMARY KEY("moud", "pregnancy"),
  FOREIGN KEY("moud") REFERENCES "moud"("id"),
  FOREIGN KEY("pregnancy") REFERENCES "pregnancy_states"("id")
);
CREATE TABLE IF NOT EXISTS "overdoses" (
  "pregnancy" INTEGER NOT NULL,
  "moud" INTEGER NOT NULL,
  "drug_behavior" INTEGER NOT NULL,
  "overdose_probability" REAL NOT NULL DEFAULT 0.0,
  "cost" REAL NOT NULL DEFAULT 0.0,
  "utility" REAL NOT NULL DEFAULT 1.0,
  PRIMARY KEY("pregnancy", "moud", "drug_behavior"),
  FOREIGN KEY("moud") REFERENCES "moud"("id"),
  FOREIGN KEY("pregnancy") REFERENCES "pregnancy_states"("id"),
  FOREIGN KEY("drug_behavior") REFERENCES "drug_behaviors"("id")
);
COMMIT;
