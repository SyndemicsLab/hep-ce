import pandas as pds
import re


def old2new(old_path, new_path, num_rows=-1):
    df = pds.read_csv(
        old_path, index_col=False)

    df.rename(columns={"age_months": "age", "gender": "sex", "idu": "drugBehaviorClassification", "time_in_former_idu_months": "timeLastActiveDrugUse", "seropositive": "seropositivity",
              "infection": "isGenotypeThree", "fibrosis": "fibrosisState", "known_status_flag": "identifiedAsPositiveInfection", "linkage": "linkageState"}, inplace=True)
    df["id"] = df["id"].str.replace('\W', '', regex=True).astype(int)
    df.drop(columns="birth_cohort_flag", inplace=True)

    idu_map = {"former": "FORMER_INJECTION",
               "none": "NEVER", "current": "INJECTION"}
    sero_map = {1: True, 0: False}
    geno_map = {"chronic-three": True, "chronic-one": False}
    df.replace({"drugBehaviorClassification": idu_map,
               "seropositivity": sero_map, "isGenotypeThree": geno_map}, inplace=True)
    df[:num_rows].to_csv(
        new_path, index=False)


if __name__ == "__main__":
    num_rows = 10000000
    old_path = "/home/matt/Repos/TestData/HEP-CE/oldinputs/hepce-inputs/shared_data/init_cohort.csv"
    new_path = f"/home/matt/Repos/TestData/HEP-CE/newinputs/init_cohort_{num_rows}.csv"
    old2new(old_path, new_path, num_rows)
