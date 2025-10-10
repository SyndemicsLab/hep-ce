/*
 * Filename: /home/matt/Repos/hep-ce/extras/examples/inputs.db.sql
 * Path: /home/matt/Repos/hep-ce/extras/examples
 * Created Date: Thursday, June 12th 2025, 2:53:18 pm
 * Author: Matthew Carroll
 * 
 * Copyright (c) 2025 Syndemics Lab
 */

BEGIN TRANSACTION;
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
COMMIT;
