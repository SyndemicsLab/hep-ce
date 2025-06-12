/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "HEP-CE", "index.html", [
    [ "Hepatitis-C Cost Effectiveness", "index.html", "index" ],
    [ "Installation", "md_docs_src_installation.html", [
      [ "Depedencies", "md_docs_src_installation.html#autotoc_md10", null ],
      [ "Installing From Source", "md_docs_src_installation.html#autotoc_md11", null ],
      [ "Installing with CMake", "md_docs_src_installation.html#autotoc_md12", null ]
    ] ],
    [ "Managing the Data", "md_docs_src_data.html", [
      [ "Required Inputs", "md_docs_src_data.html#autotoc_md14", [
        [ "sim.conf", "md_docs_src_data.html#autotoc_md15", [
          [ "simulation", "md_docs_src_data.html#autotoc_md16", null ],
          [ "mortality", "md_docs_src_data.html#autotoc_md17", null ],
          [ "infection", "md_docs_src_data.html#autotoc_md18", null ],
          [ "eligibility", "md_docs_src_data.html#autotoc_md19", null ],
          [ "fibrosis", "md_docs_src_data.html#autotoc_md20", null ],
          [ "fibrosis_staging", "md_docs_src_data.html#autotoc_md21", null ],
          [ "screening", "md_docs_src_data.html#autotoc_md22", null ],
          [ "screening_background_ab", "md_docs_src_data.html#autotoc_md23", null ],
          [ "screening_background_rna", "md_docs_src_data.html#autotoc_md24", null ],
          [ "screening_intervention_ab", "md_docs_src_data.html#autotoc_md25", null ],
          [ "screening_intervention_rna", "md_docs_src_data.html#autotoc_md26", null ],
          [ "linking", "md_docs_src_data.html#autotoc_md27", null ],
          [ "treatment", "md_docs_src_data.html#autotoc_md28", null ],
          [ "cost", "md_docs_src_data.html#autotoc_md29", null ]
        ] ],
        [ "Input Database", "md_docs_src_data.html#autotoc_md30", [
          [ "antibody_testing", "md_docs_src_data.html#autotoc_md31", null ],
          [ "background_impacts", "md_docs_src_data.html#autotoc_md32", null ],
          [ "background_mortality", "md_docs_src_data.html#autotoc_md33", null ],
          [ "behavior_impacts", "md_docs_src_data.html#autotoc_md34", null ],
          [ "behavior_transitions", "md_docs_src_data.html#autotoc_md35", null ],
          [ "bool_lookup", "md_docs_src_data.html#autotoc_md36", null ],
          [ "drug_behaviors", "md_docs_src_data.html#autotoc_md37", null ],
          [ "fibrosis", "md_docs_src_data.html#autotoc_md38", null ],
          [ "fibrosis_diagnosis_states", "md_docs_src_data.html#autotoc_md39", null ],
          [ "fibrosis_states", "md_docs_src_data.html#autotoc_md40", null ],
          [ "hcv_impacts", "md_docs_src_data.html#autotoc_md41", null ],
          [ "incidence", "md_docs_src_data.html#autotoc_md42", null ],
          [ "init_cohort", "md_docs_src_data.html#autotoc_md43", null ],
          [ "link_states", "md_docs_src_data.html#autotoc_md44", null ],
          [ "lost_to_follow_up", "md_docs_src_data.html#autotoc_md45", null ],
          [ "moud", "md_docs_src_data.html#autotoc_md46", null ],
          [ "pregnancy", "md_docs_src_data.html#autotoc_md47", null ],
          [ "pregnancy_states", "md_docs_src_data.html#autotoc_md48", null ],
          [ "screening_and_linkage", "md_docs_src_data.html#autotoc_md49", null ],
          [ "sex", "md_docs_src_data.html#autotoc_md50", null ],
          [ "smr", "md_docs_src_data.html#autotoc_md51", null ],
          [ "treatment_initiations", "md_docs_src_data.html#autotoc_md52", null ],
          [ "treatments", "md_docs_src_data.html#autotoc_md53", null ]
        ] ]
      ] ]
    ] ],
    [ "The Discrete Events", "md_docs_src_events.html", [
      [ "Aging", "md_docs_src_events.html#autotoc_md55", null ],
      [ "Behavior Changes", "md_docs_src_events.html#autotoc_md56", null ],
      [ "Clearance", "md_docs_src_events.html#autotoc_md57", null ],
      [ "Fibrosis Progression", "md_docs_src_events.html#autotoc_md58", null ],
      [ "HCV Infection", "md_docs_src_events.html#autotoc_md59", null ],
      [ "HCV Screening", "md_docs_src_events.html#autotoc_md60", null ],
      [ "HCV Linking", "md_docs_src_events.html#autotoc_md61", null ],
      [ "Voluntary Relinking", "md_docs_src_events.html#autotoc_md62", null ],
      [ "Fibrosis Staging", "md_docs_src_events.html#autotoc_md63", null ],
      [ "HCV Treatment", "md_docs_src_events.html#autotoc_md64", null ],
      [ "Death", "md_docs_src_events.html#autotoc_md65", null ]
    ] ],
    [ "Running the Model", "md_docs_src_run.html", [
      [ "Using the Executable", "md_docs_src_run.html#autotoc_md67", null ],
      [ "Arguments", "md_docs_src_run.html#autotoc_md68", null ]
    ] ],
    [ "Limitations", "md_docs_src_limitations.html", [
      [ "Library", "md_docs_src_limitations.html#autotoc_md70", null ],
      [ "Data", "md_docs_src_limitations.html#autotoc_md71", null ]
    ] ],
    [ "FAQs", "md_docs_src_faq.html", null ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", null ],
        [ "Variables", "functions_vars.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"aging_8hpp.html",
"classhepce_1_1model_1_1Person.html#af36d781b0e401e54ee6bbbc942f72d60",
"namespacehepce_1_1data.html#a6851b547514312e9f8c27813e2a2cdae",
"structhepce_1_1data_1_1MOUDDetails.html#ac9c5d9fceff527f64024a7b36e9972d2",
"types_8hpp.html#a088c53e37c90368d42dfedc9e5f6ff2e"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';