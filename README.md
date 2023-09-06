# HEP-CE Simulation
## Model Flow Structure
At the top level, the model will be broken down into three categories of events:

1. Person-Level Events
2. Clinical Events
3. Calculation Events

Each person simulated in the model cohort iterates through these event categories in this order.
Each of these categories themself contains discrete events, explained in detail below.

### Person-Level Events
Person-level events are events external to the healthcare setting that can impact the likelihood or severity of an HCV infection, such as injection drug use, age, or, if already infected, HCV progression.

Components of this event category are

- [ ] Creation
- [ ] Birth
- [X] Aging
- [ ] Opioid Drug Use Behavior
- [ ] HCV Infection
- [ ] HCV Progression (if already infected)

### Clinical Events
Clinical events are those within the healthcare setting, namely screening (testing), linkage, and treatment.

- Intervention Screening
- Background Screening
- Linkage
- Relinkage
- Treatment

### Calculation Events
- Cost
- Transmission (Infection Rate Change)
