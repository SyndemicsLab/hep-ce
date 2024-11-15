import pandas as pds
from collections import OrderedDict


def extractor(df: pds.DataFrame) -> None:
    data = OrderedDict()
    data["number of treatment initiations"] = df["completedTreatments"].sum(
    ) + df["treatmentWithdrawals"].sum()
    data["number of SVR cases"] = df["svrs"].sum()
    data["number of EOT cases(including SVRs)"] = df["completedTreatments"].sum(
    )
    data["number of non-toxicity treatment withdrawal cases"] = df["treatmentWithdrawals"].sum()
    data["number of toxicity cases"] = df["treatmentToxicReactions"].sum()
    treatments = df["treatmentWithdrawals"] + df["completedTreatments"]

    for idx, value in treatments.value_counts().items():
        st = f"number of people with {idx} treatment initializations"
        data[st] = value

    for idx, value in df["numABTests"].value_counts().items():
        st = f"number of people with {idx} antibody tests"
        data[st] = value

    for idx, value in df["numRNATests"].value_counts().items():
        st = f"number of people with {idx} RNA tests"
        data[st] = value

    for idx, value in df["linkCount"].value_counts().items():
        st = f"number of people with {idx} linkage"
        data[st] = value

    fibrosis_states = df["fibrosisState"].value_counts()
    cirrhotic_count = fibrosis_states["f4"] + fibrosis_states["decomp"]
    data["number of cirrhotic people"] = cirrhotic_count
    data["number of liver related deaths"] = df["deathReason"].value_counts()[
        "liver"]
    data["number of HCV identifications"] = df["identifiedHCV"].sum()
    data["number of new HCV infections"] = df["timesInfected"].sum()
    data["number of acute infection clearance"] = df["timesCleared"].sum()
    data["total number of ab screenings"] = df["numABTests"].sum()
    data["total number of rna screenings"] = df["numRNATests"].sum()
    data["total number of linkages"] = df["linkCount"].sum()

    result_df = pds.DataFrame([data]).T
    print(result_df.head())
    result_df.to_csv("temp.csv")


if __name__ == "__main__":
    # fix for arg path
    # file_path = "/home/matt/population.csv"
    file_path = "/home/matt/Repos/TestData/HEP-CE/newinputs/fibrosis/output2/population.csv"
    # file_path = "/projectnb/hep-ce/data/output2/population.csv"
    df = pds.read_csv(file_path)
    extractor(df)
