"""
This script processes HEP-CE simulation output data.

It should be run from your project directory which should contain folders named with the pattern "outputN"
(e.g., output1, output2, ..., outputN), each representing a different run strategy. 
For each output folder, the script expects:
  - A corresponding input database at 'inputN/inputs.db'
  - A 'population.csv' file containing individual-level simulation results in outputN

For each strategy:
  - Key metrics (e.g., costs, QALYs, infection outcomes) are extracted from 'population.csv' and the input SQL database
  - These metrics are saved to a strategy-specific summary CSV file (e.g., general_statsN.csv) in the outputN directory
  - These general_statsN.csv files are then process into a single combined summary table ("organized_outputs.csv"), with one row per strategy run

Run this script from the command line or Python environment while your working directory is set to the project folder containing the "outputN" folders.

Dependencies:
  - pandas
  - sqlite3
  - csv
  - re
  - os

Output:
  - One CSV per strategy: general_statsN.csv
  - A master summary table: organized_outputs.csv
"""

import os
import csv
import re
import pandas as pds
import sqlite3
from collections import OrderedDict

# === SETTINGS ===
OUT_FOLDER_PREFIX = "output"
FILENAME_PREFIX = "general_stats"
FILENAME_SUFFIX = ".csv"
TABLE_NAME = "organized_outputs.csv"

# === Data Points to Include in Summary Table ===
DATAPOINTS = [
    "Avg Life Span per person in Years",
    "Avg Cost per person in USD",
    "Avg Discounted Cost per person in USD",
    "Avg QALY per person minimal method",
    "Avg QALY per person multiplicative method",
    "Avg Discounted QALY per person minimal method",
    "Avg Discounted QALY per person multiplicative method",
    "number of total HCV infections",
    "number of total HCV identifications",
    "number of SVR cases",
    "number of cirrhotic people",
    "number of liver related deaths"
]

# === EXTRACTOR FUNCTION ===
def extractor(df: pds.DataFrame, output_filename: str, input_db_path: str) -> None:
    """Extracts key HCV statistics from the dataframe and saves them as a CSV."""
    data = OrderedDict()

    # --- Query initial infection and identification status from the SQLite database ---
    if os.path.exists(input_db_path):
        with sqlite3.connect(input_db_path) as conn:
            cursor = conn.cursor()

            # Initial infections
            cursor.execute("""
                SELECT COUNT(*) FROM init_cohort
                WHERE hcv_status IN (1, 2)
            """)
            initial_infections = cursor.fetchone()[0] or 0

            # Initial identifications
            cursor.execute("SELECT SUM(identified_as_hcv_positive) FROM init_cohort")
            initial_identified = cursor.fetchone()[0] or 0
    else:
        print(f"Warning: {input_db_path} not found. Setting initial infections and identifications to 0.")
        initial_infections = 0
        initial_identified = 0

    # --- Basic counts ---
    data["number of treatment initiations"] = df["num_completed_hcv_treatments"].sum() + df["num_hcv_treatment_withdrawals"].sum()
    data["number of SVR cases"] = df["svrs"].sum()
    data["number of EOT cases (including SVRs)"] = df["num_completed_hcv_treatments"].sum()
    data["number of non-toxicity treatment withdrawal cases"] = df["num_hcv_treatment_withdrawals"].sum()
    data["number of toxicity cases"] = df["num_hcv_treatment_toxic_reactions"].sum()

    treatments = df["num_hcv_treatment_withdrawals"] + df["num_completed_hcv_treatments"]
    for idx, value in treatments.value_counts().items():
        data[f"number of people with {idx} treatment initializations"] = value

    for idx, value in df["num_hcv_ab_tests"].value_counts().items():
        data[f"number of people with {idx} antibody tests"] = value

    for idx, value in df["num_hcv_rna_tests"].value_counts().items():
        data[f"number of people with {idx} RNA tests"] = value

    for idx, value in df["hcv_link_count"].value_counts().items():
        data[f"number of people with {idx} linkage"] = value

    data["Avg Life Span per person in Years"] = df["life_span"].mean() / 12
    fibrosis_states = df["fibrosis_state"].value_counts()
    cirrhotic_count = fibrosis_states.get("f4", 0) + fibrosis_states.get("decomp", 0)
    data["number of cirrhotic people"] = cirrhotic_count
    data["number of liver related deaths"] = df["death_reason"].value_counts().get("liver", 0)

    # --- HCV infection breakdown ---
    incident_infections = int(df["times_hcv_infected"].sum())
    acute_clearances = int(df["times_acute_cleared"].sum())
    total_infections = initial_infections + incident_infections

    data["number of initial HCV infections"] = initial_infections
    data["number of incident HCV infections"] = incident_infections
    data["number of total HCV infections"] = total_infections

    # --- HCV identification breakdown ---
    data["number of initial HCV identifications"] = initial_identified

    incident_identified = incident_infections - len(df[(df["hcv_identified"] == False) &
                                                      (df["hcv"].isin(["acute", "chronic"]))])

    total_identified = initial_identified + incident_identified

    data["number of incident HCV identifications"] = incident_identified
    data["number of total HCV identifications"] = total_identified

    # --- Screening and linkage totals ---
    data["number of acute infection clearance"] = acute_clearances
    data["total number of ab screenings"] = df["num_hcv_ab_tests"].sum()
    data["total number of rna screenings"] = df["num_hcv_rna_tests"].sum()
    data["total number of linkages"] = df["hcv_link_count"].sum()

    # --- Cost-effectiveness calculations ---
    data["Avg Cost per person in USD"] = df["cost"].mean()
    data["Avg Discounted Cost per person in USD"] = df["discount_cost"].mean()

    # --- QALY calculations ---
    data["Avg QALY per person minimal method"] = (df["min_utility"]/12).mean()
    data["Avg Discounted QALY per person minimal method"] = (df["discounted_min_utility"]/12).mean()
    data["Avg QALY per person multiplicative method"] = (df["min_utility"]/12).mean()
    data["Avg Discounted QALY per person multiplicative method"] = (df["discounted_min_utility"]/12).mean()

    # --- Save to CSV ---
    result_df = pds.DataFrame([data]).T
    print(f"Saving results to {output_filename}")
    result_df.to_csv(output_filename)

# === VALUE EXTRACTION FROM FILE ===
def extract_value_from_file(filepath, datapoint):
    with open(filepath, newline='') as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            if row and re.sub(r'\s+', '', row[0].strip('"')) == re.sub(r'\s+', '', datapoint):
                return row[1].strip()
    return "NA"

# === AGGREGATE SUMMARY TABLE CREATION ===
def aggregate_outputs(root_dir):
    organized_rows = []
    header = ["strategy", "run number"] + DATAPOINTS
    organized_rows.append(header)

    strategy_run_counts = {}

    for folder_name in sorted(os.listdir(root_dir)):
        if folder_name.startswith(OUT_FOLDER_PREFIX) and os.path.isdir(os.path.join(root_dir, folder_name)):
            strategy_id = folder_name[len(OUT_FOLDER_PREFIX):]
            folder_path = os.path.join(root_dir, folder_name)
            population_csv = os.path.join(folder_path, "population.csv")
            input_db_path = os.path.join(root_dir, f"input{strategy_id}", "inputs.db")
            general_stats_file = os.path.join(folder_path, f"{FILENAME_PREFIX}{strategy_id}{FILENAME_SUFFIX}")

            if not os.path.exists(population_csv):
                print(f"Warning: {population_csv} not found. Skipping {folder_name}.")
                continue

            df = pds.read_csv(population_csv)
            extractor(df, general_stats_file, input_db_path)

            run_count = strategy_run_counts.get(strategy_id, 0) + 1
            strategy_run_counts[strategy_id] = run_count

            row = [strategy_id, run_count]
            for dp in DATAPOINTS:
                val = extract_value_from_file(general_stats_file, dp)
                row.append(val)

            organized_rows.append(row)
            
            print(f"Adding results to {TABLE_NAME}")

    with open(TABLE_NAME, "w", newline='') as out_csv:
        writer = csv.writer(out_csv)
        writer.writerows(organized_rows)

# === MAIN ===
if __name__ == "__main__":
    base_dir = os.getcwd()
    aggregate_outputs(base_dir)
