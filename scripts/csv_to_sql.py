import sqlite3
import pandas as pd

DB_FILE = "/home/matt/Repos/TestData/HEP-CE/newinputs/input1/inputs.db"
conn = sqlite3.connect(DB_FILE)
INPUT_DIR = "/home/matt/Repos/TestData/HEP-CE/newinputs/input1"
FILENAME = "init_cohort"

drug_behaviors = {"never": 0, "former_noninjection": 1,
                  "former_injection": 2, "noninjection": 3, "injection": 4}
bool_map = {"False": 0, "True": 1}
fibrosis_states = {"f0": 0, "f1": 1, "f2": 2, "f3": 3, "f4": 4, "decomp": 5}
sex_map = {"male": 0, "female": 1}
fib_dig_states = {"f01": 0, "f23": 1, "f4": 2, "decomp": 3}
moud_map = {"none": 0, "current": 1, "post": 2}
pos_neg_map = {"positive": 1, "negative": 0}
link_map = {"never": 0, "linked": 1, "unlinked": 2}
columns_map = {"age": "age_months", "sex": "gender", "drugBehaviorClassification": "drug_behavior", "timeLastActiveDrugUse": "time_last_active_drug_use", "isGenotypeThree": "genotype_three",
               "fibrosisState": "fibrosis_state", "identifiedAsPositiveInfection": "identified_as_hcv_positive", "linkageState": "link_state"}


def rename_cols(df: pd.DataFrame) -> pd.DataFrame:
    df.drop(columns=["id"], inplace=True)
    df["drugBehaviorClassification"] = df["drugBehaviorClassification"].str.lower()
    df.rename(columns=columns_map, inplace=True)
    return df


def process_file(df: pd.DataFrame):
    if "drug_behavior" in df.columns:
        df["drug_behavior"] = df["drug_behavior"].map(drug_behaviors)
    if "gender" in df.columns:
        df["gender"] = df["gender"].map(sex_map)
    if "moud" in df.columns:
        df["moud"] = df["moud"].map(moud_map)
    if "fibrosis_state" in df.columns:
        df["fibrosis_state"] = df["fibrosis_state"].map(fibrosis_states)
    if "seropositivity" in df.columns:
        df["seropositivity"] = df["seropositivity"].map({True: 1, False: 0})
    if "genotype_three" in df.columns:
        df["genotype_three"] = df["genotype_three"].map(
            {"True": 1, "False": 0, "chronic-two": 0})
    if "cirrhotic" in df.columns:
        df["cirrhotic"] = df["cirrhotic"].map({True: 1, False: 0})
    if "link_state" in df.columns:
        df["link_state"] = df["link_state"].map(link_map)

    # print(len(df))
    # print(df.head())
    df.to_sql(name=FILENAME, con=conn,
              index=False, if_exists="append")


dataframe = rename_cols(pd.read_csv(INPUT_DIR + "/" + FILENAME + ".csv"))
process_file(dataframe)
conn.commit()
conn.close()
