# Technical Writing Samples

These are curated excerpts from Sean Balbale's lab reports across ENGR 200 (Trinity), ENGR 305 (Trinity), and BIOL/ECE labs (Northeastern). These represent Sean's most polished technical voice. Pay close attention to: active voice, cross-lab narrative threading, assertive discussion tone, precision with SI units, and the three-column comparison table format.

---

## Sample 1: Lab Report Introduction — Narrative Threading (ENGR 200, Lab 7)

> The accurate characterization of physical phenomena relies not only on the fidelity of the sensors employed but on the robust engineering of the data acquisition (DAQ) and signal conditioning pipeline. Over the past four experimental modules, we have systematically evaluated the constraints of this pipeline. Lab 4 established the fundamentals of digital signal acquisition and spectral analysis via Fast Fourier Transforms (FFT). Lab 5 introduced the strict mathematical boundaries of digitization, demonstrating how undersampling vitiates high-frequency data (aliasing) and how finite 16-bit hardware resolution distorts low-amplitude signals through quantization error. To bridge this gap between low-voltage physical signals and DAQ resolution limits, Lab 6 explored analog signal conditioning using the TL072 operational amplifier, mapping the physical limits of amplifier clipping and supply rail saturation.

### What to notice:
- **Narrative threading**: References Labs 4, 5, and 6 before introducing Lab 7. Every report is a chapter in a larger story.
- **Academic vocabulary in a technical context**: "vitiates high-frequency data."
- **Active constructions**: "we have systematically evaluated," "Lab 5 introduced."
- **"Bridge" usage**: "To bridge this gap between low-voltage physical signals and DAQ resolution limits."
- **Parenthetical definitions**: "(aliasing)" after describing the phenomenon.

---

## Sample 2: Lab Report Objective — Bullet List Format (ENGR 305, Lab 10 — BJT CE Amplifier)

> The objective of this laboratory exercise is to design, simulate, and characterize a BJT Common-Emitter (CE) amplifier. The specific goals are:
> - To design a CE amplifier with a specific quiescent current (IC = 1 mA) and a target voltage gain of |Av| = 200 V/V.
> - To perform a SPICE simulation to verify the DC operating point and AC small-signal performance.
> - To build the circuit and measure the experimental gain, input/output impedances, and signal limits (clipping).
> - To compare theoretical calculations, simulation results, and experimental measurements.

### What to notice:
- **States four sub-goals**, each with a concrete deliverable (not "understand" or "explore").
- **Parallel bullet construction**: "To design... To perform... To build... To compare..."
- **Quantitative targets embedded in the objective**: "IC = 1 mA," "|Av| = 200 V/V" — not vague goals.

---

## Sample 3: DC Design Rationale — Showing the Work (ENGR 305, Lab 10)

> The circuit was designed to meet the following specifications: VCC = +15V, VEE = −15V, Load Resistance RL = 10 kΩ, Quiescent Current IC = 1 mA, Magnitude of Voltage Gain |Av| = 200.
>
> Calculating RE: To achieve IC ≈ IE = 1 mA with the base at ground:
> RE = (0V − 0.7V − (−15V)) / 1 mA = 14.3V / 1 mA = 14.3 kΩ (Standard value chosen: 14.3 kΩ).
>
> Calculating RC: The transconductance is gm = 1 mA / 26 mV = 38.46 mS. Using the gain equation |Av| = gm(RC ∥ RL):
> 200 = (38.46 × 10⁻³)(RC · 10k / (RC + 10k))
> Solving for RC ≈ 10.83 kΩ (Standard value chosen: 10.8 kΩ).

### What to notice:
- **Shows the algebra at every step** — not just a final answer.
- **Notes the standard resistor value chosen** — documents the engineering compromise.
- **Lists all specifications upfront** so the design is traceable.

---

## Sample 4: The Three-Column Comparison Table (ENGR 305, Lab 10)

> **Table 1: DC Operating Point Analysis: Comparison of Calculated, Simulated, and Measured Values**
>
> | Parameter | Calculated | Simulated | Measured |
> |-----------|-----------|-----------|---------|
> | VC | 4.170 V | 4.010 V | 4.170 V |
> | VE | −0.700 V | −0.687 V | −0.718 V |
> | VBE (VB − VE) | 0.700 V | 0.655 V | 0.701 V |
> | VCE (VC − VE) | 4.870 V | 4.700 V | 4.888 V |
> | IC | 1.000 mA | 1.010 mA | 1.010 mA |
> | IE | 1.000 mA | 1.010 mA | 1.010 mA |

### What to notice:
- **Three columns every time**: Calculated (design goal), Simulated (SPICE), Measured (lab). This is Sean's standard structure for any experimental comparison.
- **Formulas in column headers**: "VBE (VB − VE)" — the reader knows exactly how it was computed.
- **Every parameter has units.**

---

## Sample 5: Discussion With Sub-Headers (ENGR 305, Lab 10)

> **Analysis of DC Values:**
> - **VBE:** The measured Base-Emitter voltage (0.701 V) is almost exactly the standard theoretical turn-on voltage of 0.7 V. The simulation predicted a slightly lower turn-on voltage (≈ 0.65 V), which is typical for the SPICE model at low currents.
> - **VCE:** The measured Collector-Emitter voltage (4.888 V) matches the design goal (4.87 V) exceptionally well, confirming the transistor is biased correctly in the forward-active region.
> - **Currents:** The measured currents (IC and IE) are within 1% of the 1 mA design target.
>
> **Gain Analysis:** The measured gain of −196.8 V/V is incredibly close to the design goal of 200 (error < 1.6%).
> - vs. Calculation: The match is near perfect, suggesting the transconductance estimate (gm ≈ IC/VT) held true.
> - vs. Simulation: The measured gain was slightly higher than the simulated gain (190). The simulation likely assumes a lower Early Voltage (VA) for the generic 2N3904 model, which would lower the effective output impedance and thus the gain.

### What to notice:
- **Bold sub-headers within the discussion**: "Analysis of DC Values:", "Gain Analysis:" — structured like a lab notebook, not an essay.
- **Percentage error is always stated**: "error < 1.6%."
- **Comparison is structured**: "vs. Calculation" / "vs. Simulation" — not a wall of text.
- **Explains differences causally**: "The simulation likely assumes a lower Early Voltage" — not "there was a small difference."

---

## Sample 6: Diagnosing a Discrepancy — NMOS Lab (ENGR 305, Lab 9)

> **Voltage Gain (Av):** The measured gain Av = −3.285 V/V was substantially lower than the calculated goal (−5.0 V/V) and the simulation (−5.487 V/V). The gain is given by Av = −gm(Req), where Req = RD ∥ ro ∥ RL. The measured Req (using measured component values) is ≈ 3.42 kΩ, which is very close to the design value. This implies that the discrepancy must come from the transconductance, gm.
>
> The measured gm can be estimated as |Av|/Req = 3.285/3.42 kΩ ≈ 0.96 mS. This is much lower than the calculated gm = 1.47 mS. Since gm = √(2knID) and the measured ID was accurate, this strongly suggests that the actual process transconductance parameter (kn) of the transistor was significantly lower than the 1.08 mA/V² value used in the design.

### What to notice:
- **Isolates the variable causing the discrepancy**: eliminates Req as the cause, then proves gm is the culprit.
- **Back-calculates the actual device parameter** from measurements — this is the assertive engineering approach.
- **"This implies... this strongly suggests..."** — calibrated confidence. Uses "conclusively proves" only when the evidence is airtight.

---

## Sample 7: Output Resistance Measurement (ENGR 305, Lab 10)

> The output resistance was determined experimentally by comparing the "Open Load" output voltage to the "Loaded" output voltage.
>
> Calculation: Using the voltage divider relationship:
> vloaded = vopen × RL / (RL + Ro)
>
> Solving for Ro:
> Ro = RL × (vopen/vloaded − 1) = 9.1 kΩ × (9.60/4.32 − 1) ≈ 11.12 kΩ
>
> Comparison: This experimental value (11.1 kΩ) is very close to the measured collector resistor value of RC = 10.74 kΩ. This confirms the theory that for a BJT amplifier, Ro ≈ RC (assuming ro is large).

### What to notice:
- **Shows the derivation step-by-step**, even for a simple voltage divider.
- **Plugs in actual measured values**, not nominal values.
- **Connects result to theory**: "This confirms the theory that... Ro ≈ RC."

---

## Sample 8: Lab Report Introduction — Biomedical Context (ECG Circuit, Northeastern)

> Instrumentation amplifiers served as critical components in many biomedical applications due to their ability to amplify small differential signals while rejecting unwanted noise, specifically common-mode noise. In the context of electrocardiogram (ECG) signal acquisition, the signals of interest were often in the millivolt range and could easily be overshadowed by noise from the environment, including 60 Hz AC interference, electromagnetic coupling, and motion artifacts. Instrumentation amplifiers, such as the AD627, addressed these challenges by providing high differential-mode gain while maintaining high input impedance and excellent common-mode rejection.

### What to notice:
- **Opens with the engineering rationale** for the device choice, not just what the lab is about.
- **Names the specific noise sources** (60 Hz, electromagnetic coupling, motion) — not a generic "noise."
- **Names the part** (AD627) immediately with context.

---

## Sample 9: CMRR Calculation with Context (ECG Lab)

> The common-mode gain Acm was measured by applying the same signal to both inputs of the amplifier.
>
> Given: Ad = 24.50, Acm = 0.0002
>
> CMRR = 20 · log₁₀(Ad/Acm) = 20 · log₁₀(24.50/0.0002) = 81.94 dB
>
> This result is close to the AD627's specifications of 77 dB.

### What to notice:
- **States the formula before plugging in values.**
- **Lists the given values explicitly** so the calculation is traceable.
- **Compares to datasheet spec** — this is always the final step.

---

## Sample 10: Discussion — Assertive Analysis (ENGR 200, Lab 7)

> The empirical Bode plot generated in Fig. 2 makes complete physical sense for the analyzed circuit architecture. The magnitude ratio exhibits a flat passband, remaining constant at approximately 19.10 dB from 1 Hz through to 10000 Hz. This stability proves the amplifier is operating purely within its mid-band frequencies without attenuation.

### What to notice:
- **"Makes complete physical sense"** — assertive, not hedged.
- **"Proves"** — Sean doesn't write "suggests" or "appears to" when the data is clear.
- **References the specific figure**: "in Fig. 2."

---

## Sample 11: Ruling Out Alternative Explanations (ENGR 200, Lab 7)

> It is mathematically incorrect to attribute this discrepancy to DAQ loading impedances, as the NI-6321 possesses an input impedance of >10 GOhm, making any breadboard loading effects on a 10 kΩ circuit entirely negligible.

### What to notice:
- **"It is mathematically incorrect to attribute..."** — eliminates wrong explanations actively.
- **Backs it up with the ratio**: 10 GOhm input vs. 10 kΩ circuit — three orders of magnitude.
- **"Entirely negligible"** — confident dismissal, supported by the number.

---

## Sample 12: Conclusion Structure (ENGR 305, Lab 10)

> The design of the Common-Emitter amplifier was highly successful.
> 1. The DC bias point was stable with IC = 1.01 mA, matching the 1 mA target. The measured VBE and VCE values matched theoretical expectations almost perfectly.
> 2. The measured AC gain was −196.8 V/V, which is within 1.6% of the −200 V/V goal.
> 3. The output resistance was measured to be 11.12 kΩ, confirming it is dominated by the collector resistor (RC).

### What to notice:
- **Opens with a one-sentence verdict**: "The design... was highly successful."
- **Numbered list** matches the objective list from the Introduction — satisfying structural closure.
- **Each conclusion item has a number**: "1.6% of the −200 V/V goal" — never vague.

---

## Writing Patterns Summary

| Pattern | Technical Expression |
|---------|---------------------|
| Assertive conclusion | "This conclusively proves," "makes complete physical sense," "confirms the theory that" |
| Eliminating wrong answers | "It is mathematically incorrect to attribute X to Y, as [reason]." |
| Diagnosing discrepancy | Eliminate each variable in turn; back-calculate the root cause from measured data |
| Comparison table | Three columns: Calculated / Simulated / Measured. Always include percentage error. |
| Discussion sub-headers | "Analysis of [Parameter] Values:" followed by bolded sub-items |
| Objective statement | Parallel "To [verb]..." bullets with quantitative targets embedded |
| Conclusion opener | One-sentence verdict, then numbered list matching the objectives |
| Cross-lab reference | "Drawing directly from the principles established in Lab [N]..." |

---

## Sample 13: ENGR 305 Report Structure — Section Architecture

The ENGR 305 (Trinity, Prof. Fixel) lab reports follow a **different structure** from the Northeastern labs. Use this for any ENGR 305 report.

**ENGR 305 section order:**
1. `\section{Objective}` — bullet list of specific goals with quantitative targets
2. `\section{Theory}` — derives the relevant equations using `\paragraph{}` sub-headings (DC Analysis, AC Analysis, etc.)
3. `\section{Experimental Method and Design}` (or "Experimental Method and Reasoning") — explains the process and design choices *before* results, organized by subsection per part
4. `\section{Results and Conclusions (Discussion)}` — contains sub-subsections per part: Hand Calculations → Simulation Results → Measurement Data → Post-Measurement Calculations → Comparison Table
5. `\subsection{Discussion}` — three named `\paragraph{}` blocks: "Comparison: Hand Calculations vs. Simulation vs. Datasheet", "Comparison: Simulation vs. Measurement vs. Datasheet", "Overall Performance"
6. `\subsection{Conclusion}` — one-sentence verdict + bullet list of key findings with measured values
7. `\section{Bibliography}` — numbered, with full citation including datasheet URLs

**Northeastern section order (for contrast):**
1. `\section{Introduction}`
2. `\section{Results}`
3. `\section{Discussion and Conclusion}` *(combined)*
4. `\section{References}`

---

## Sample 14: The \paragraph{Given Parameters} Block (ENGR 305)

Every "Hand Calculations" sub-subsection opens with a structured parameters block:

```latex
\paragraph{Given Parameters}
\begin{itemize}
  \item \textbf{Voltage Supplies:} $V_{+} = \SI{15}{\volt}$, $V_{-} = \SI{-15}{\volt}$
  \item \textbf{Design Goals:} $I_{C} = \SI{1}{\milli\ampere}$, $V_{B} = \SI{0}{\volt}$, $V_{C} = \SI{-5}{\volt}$
  \item \textbf{Transistor Model:} $\beta = 100$
  \item \textbf{Assumption:} Active region $V_{EB(\text{on})} = \SI{0.7}{\volt}$
\end{itemize}

\paragraph{Calculations}
$$
I_B = \frac{I_C}{\beta} = \frac{\SI{1}{\milli\ampere}}{100} = \SI{10}{\micro\ampere}
$$
```

### What to notice:
- **Bold labels** ("Design Goals:", "Assumption:") — reader can scan in 2 seconds.
- **All values in `\SI{}{}`** — no naked numbers.
- **`\paragraph{}` introduces each logical block** within a sub-subsection — not `\subsubsection{}`.
- **"Hand Calc (Goal)"** is the column header for the hand-calculated column — it's not just "Calculated."

---

## Sample 15: Post-Measurement Calculations (ENGR 305)

After listing raw measured values, always derive the calculated quantities:

```latex
\paragraph{Post-Measurement Calculations}
Based on the measured values above:
$$
V_{EB} = V_E - V_B = \SI{1.755}{\volt} - \SI{1.09}{\volt} = \SI{0.665}{\volt}
$$
$$
I_E = \frac{V_{+} - V_E}{R_E} = \frac{\SI{15.01}{\volt} - \SI{1.755}{\volt}}{\SI{14.7}{\kilo\ohm}} = \SI{0.902}{\milli\ampere}
$$
$$
\beta = \frac{I_C}{I_B} = \frac{\SI{0.898}{\milli\ampere}}{\SI{0.004}{\milli\ampere}} \approx 225
$$
```

### What to notice:
- Uses the **measured supply voltages** (15.01 V, not 15 V) — documents the actual hardware.
- **Every intermediate step shown** with the formula → substitution → result pattern.
- **Back-calculates device parameters** (β) from measured data — this is the engineering insight.

---

## Sample 16: The booktabs Comparison Table (ENGR 305)

```latex
\begin{table}[H]
    \centering
    \caption{Part 1: Comparison of Calculated, Simulated, and Measured Values}
    \label{tab:part1_compare}
    \sisetup{round-mode=places,round-precision=3}
    \begin{tabular}{lccc}
        \toprule
        \textbf{Parameter} & \textbf{Hand Calc (Goal)} & \textbf{Simulation} & \textbf{Measured} \\
        \midrule
        $V_B$ & \SI{0}{\volt} & \SI{1.03}{\volt} & \SI{1.09}{\volt} \\
        $I_C$ & \SI{1.0}{\milli\ampere} & \num{0.936e-3} A & \num{0.898e-3} A \\
        $\beta$ ($h_{FE}$) & 100 (Assumed) & $\approx 213$ & $\approx 225$ \\
        \bottomrule
    \end{tabular}
\end{table}
```

### What to notice:
- **`\toprule`, `\midrule`, `\bottomrule`** from booktabs — no vertical lines.
- **`\caption` before `\begin{tabular}`** — caption goes above the table.
- **`\sisetup{round-mode=places,round-precision=3}`** — sets SI formatting for the whole table.
- **"(Assumed)"** and **"(Goal)"** annotations in cells — flags design assumptions directly in the table.
- **`\approx`** for back-calculated values — doesn't claim false precision.

---

## Sample 17: Three-Way Discussion Structure (ENGR 305)

The Discussion sub-section always contains three named `\paragraph{}` blocks:

```
\paragraph{Comparison: Hand Calculations vs. Simulation vs. Datasheet}
[Explain WHY simulation deviated from hand calcs. Name the specific assumptions 
that broke down. Reference datasheet specs.]

\paragraph{Comparison: Simulation vs. Measurement vs. Datasheet}
[Explain agreement/discrepancy between bench results and simulation.
Name component tolerances. Flag any anomaly as "warrants further investigation."]

\paragraph{Overall Performance}
[One-paragraph verdict. State what was achieved. What worked, what didn't.]
```

Key phrases from Sean's discussions:
- **"This was primarily due to the simplified assumptions used in the hand calculations, namely..."** — names the assumptions
- **"The higher actual β required less base current... This reduced loading on the base voltage divider caused VB to rise..."** — causal chain
- **"...warrants further investigation (potential measurement error or component issue)"** — honest about anomalies, doesn't pretend they're explained
- **"This highlights the significant variability in β even for transistors of the same part number."** — system-level takeaway

---

## Sample 18: Conclusion with Bullet Key Findings (ENGR 305)

```latex
\subsection{Conclusion}
This laboratory exercise successfully investigated the DC biasing of a 2N3906 PNP BJT 
in active, saturation, and diode-connected configurations. [...] Key findings include:
\begin{itemize}
    \item Target currents were closely achieved in all three configurations. 
          Measured $I_C$ values were \SI{0.898}{mA} (Part 1), \SI{1.018}{mA} (Part 2), 
          and $\approx \SI{1.017}{mA}$ (Part 3).
    \item Measured $V_{EB}$ values were consistently around \SI{0.66}{V}, aligning well 
          with datasheet typical values and simulations.
    \item The measured active-mode $\beta$ ($\approx 225$) was significantly higher than 
          the assumed 100, demonstrating expected device variance but falling within 
          plausible limits (above the minimum 80).
\end{itemize}
```

### What to notice:
- **One-sentence verdict** before "Key findings include:" — assertive, not hedged.
- **Every bullet has a number**: "0.898 mA (Part 1)" — not vague statements.
- **Compares to spec** ("above the minimum 80") — always closes the loop to the datasheet.
- **"Demonstrating expected device variance"** — explains the anomaly in one phrase.

---

## Sample 19: Bibliography Entry (ENGR 305)

```latex
\section{Bibliography}
[1] Fixel, Debora. "ENGR 305 - Lab 10: BJT Common-Emitter Amplifier." Trinity College, 
    Hartford, CT, November 2025.

[2] Jameco Electronics. "2N3904 NPN General Purpose Amplifier." Datasheet, Part no. 38359.

[3] ON Semiconductor. "2N3906 General Purpose Transistors PNP Silicon." Datasheet, 
    2N3906/D, Rev. 4, February 2010. http://onsemi.com.
```

### What to notice:
- **`\section{Bibliography}`** (not "References") in ENGR 305 reports.
- **Lab manual citation format**: `[Author Last, First]. "[Course - Lab Title]." [Institution], [City, State], [Month Year].`
- **Datasheet citation format**: `[Manufacturer]. "[Part Description]." Datasheet, [Part number/Rev].`
- Always includes the **datasheet** for the key component as a citation.

---

## LaTeX Package Reference (Sean's Standard Setup)

**ENGR 305 (Trinity) preamble:**
```latex
\usepackage[margin=1in]{geometry}
\usepackage{graphicx}
\usepackage{float}          % [H] figure placement
\usepackage{booktabs}       % \toprule, \midrule, \bottomrule
\usepackage{siunitx}        % \SI{}{} and \si{}
\usepackage{amsmath}
\usepackage{amssymb}
\usepackage{pgfplots}       % For Bode plots / data plots
\pgfplotsset{compat=1.18}
\usepackage{fancyhdr}
\usepackage{hyperref}
\usepackage{circuitikz}     % For schematic drawings
\usepackage{subcaption}     % For subfigures
\setlength{\parindent}{0in}
\setlength{\parskip}{\baselineskip}
```

**Northeastern preamble (earlier labs):**
```latex
\usepackage[margin=1 in]{geometry}
\usepackage{graphicx}
\usepackage{float}
\usepackage{booktabs}
\usepackage{siunitx}
\usepackage{amsmath}
\usepackage{amssymb}
\setlength{\parindent}{0in}
% Note: no \parskip — uses \newline between paragraphs instead
```

**Key difference**: ENGR 305 reports use `\setlength{\parskip}{\baselineskip}` (automatic paragraph spacing) while Northeastern reports use explicit `\newline` commands between paragraphs.

---

## Sample 13: LaTeX Document Structure — Standard Package Set (ENGR 305)

```latex
\documentclass[12pt]{article}
\usepackage[margin=1in]{geometry}
\usepackage{graphicx}
\usepackage{float}          % [H] figure placement
\usepackage{booktabs}       % \toprule, \midrule, \bottomrule
\usepackage{siunitx}        % \SI{}{} for all values with units
\usepackage{amsmath}
\usepackage{amssymb}
\usepackage{pgfplots}
\pgfplotsset{compat=1.18}
\usepackage{fancyhdr}
\usepackage{hyperref}
\usepackage{circuitikz}     % Circuit schematics
\usepackage{subcaption}     % Side-by-side subfigures

\setlength{\parindent}{0in}
\setlength{\parskip}{\baselineskip}
```

### What to notice:
- **`siunitx` is non-negotiable.** Every value with a unit uses `\SI{10.83}{k\Omega}` — never raw text like "10.83 kΩ."
- **`booktabs` replaces `\hline`.** Tables use `\toprule`, `\midrule`, `\bottomrule` — never `|` column separators or `\hline`.
- **`circuitikz`** draws all circuit schematics natively in LaTeX. No external schematic images when circuitikz can do it.
- **`subcaption`** is used when two related oscilloscope screenshots appear side-by-side (e.g., open-load vs. loaded).
- **`\parindent{0in}` + `\parskip{\baselineskip}`** — no indentation, one full line between paragraphs. This is Sean's consistent formatting choice.

---

## Sample 14: Section Hierarchy — ENGR 305 Lab Report (Lab 10)

```
\section{Objective}         % Bullet list of specific goals
\section{Theory}            % Named paragraphs: \paragraph{DC Analysis}, \paragraph{AC Analysis}
\section{Experimental Method and Design}
  \subsection{Design Procedure}   % Numbered bold steps: \textbf{1. Calculating RE:}
  \subsection{Schematic}          % circuitikz figure
  \subsection{Experimental Setup} % Photo + measured component values
\section{Results and Discussion}
  \subsection{DC Operating Point}          % Table + Analysis bullet list
  \subsection{AC Performance and Gain}     % Table + Gain Analysis bullet list
  \subsection{Output Resistance}           % Subfigures + calculation
  \subsection{Clipping and Distortion}     % Figure + explanation
\section{Conclusion}        % Numbered list matching objectives
\section{Bibliography}      % [N] numbered format
```

### What to notice:
- **Conclusion numbered list** mirrors the Objective bullet list — structural closure.
- **Design Procedure steps** are bold numbered: `\textbf{1. Calculating $R_E$:}` — not prose, not section headers.
- **Theory** uses `\paragraph{}` (named, unnumbered, inline heading) not `\subsection`.

---

## Sample 15: Section Hierarchy — Multi-Part Lab (Lab 8 — PNP at DC)

```
\section{Objective}
\section{Theory}          % Three operating regions as \begin{itemize} with \textbf{Region Name:}
\section{Experimental Method and Reasoning}
  \subsection{Part 1: PNP in Active Mode}    % Design goals + assumptions only
  \subsection{Part 2: PNP in Saturation Mode}
  \subsection{Part 3: Diode-Connected PNP}
\section{Results and Conclusions (Discussion)}
  \subsection{Part 1: Active Mode Results}
    \begin{figure}[H] ... \end{figure}       % Circuit schematic first
    \subsubsection{Hand Calculations}
      \paragraph{Given Parameters}           % \begin{itemize} of specs
      \paragraph{Calculations}               % Step-by-step equations
    \subsubsection{Simulation Results}       % \begin{itemize} of node voltages
    \subsubsection{Measurement Data}         % \begin{itemize} of measured values
    \paragraph{Post-Measurement Calculations}% Equations using measured values
    \subsubsection{Comparison of Results (Part N)} % booktabs table
  \subsection{Discussion}
    \paragraph{Comparison: Hand Calc vs. Simulation vs. Datasheet}
    \paragraph{Comparison: Simulation vs. Measurement vs. Datasheet}
    \paragraph{Overall Performance}
  \subsection{Conclusion}  % \begin{itemize} of key findings with specific numbers
```

### What to notice:
- **Circuit schematic appears before the hand calculations** in each part's Results section.
- **Two-tier discussion**: First compare hand calc to simulation (model accuracy), then compare simulation to measurement (hardware accuracy). Two separate comparisons, not one wall of text.
- **Conclusion is a bullet list with specific numbers**: "Measured IC values were 0.898 mA (Part 1), 1.018 mA (Part 2), and ≈ 1.017 mA (Part 3)."
- **`\paragraph{}`** is used for named discussion sub-items within `\subsection{Discussion}`.

---

## Sample 16: Thevenin Base Biasing — 3-Step Design Method (Lab 8)

> **Calculate Base Biasing Resistors (R1, R2) using Thevenin's Theorem:**
>
> The problem is not fully specified, requiring a choice for the Thevenin resistance (Rth) of the base biasing network. A guideline for stability is Rth ≤ 0.1(β+1)RE.
>
> 1. *Choose Rth:* The guideline suggests Rth ≤ 0.1(101)(14.16 kΩ) ≈ 143 kΩ. Choose Rth = 75 kΩ.
> 2. *Calculate Vth:* Using VB = Vth − IB·Rth with VB = 0 V and IB = 10 μA:
>    0 V = Vth − (10 μA)(75 kΩ) → Vth = 0.75 V
> 3. *Solve for R1 and R2:* Using Vth = R2/(R1+R2)·(V+−V−) + V− and Rth = R1R2/(R1+R2):
>    → R1 ≈ 142.9 kΩ, R2 ≈ 157.9 kΩ

### What to notice:
- **Always state the stability guideline** before choosing Rth: `Rth ≤ 0.1(β+1)RE`.
- **Enumerated 3-step structure**, not prose: Choose → Calculate → Solve.
- **Italic step labels**: `\textit{Choose Rth:}` — distinguishes the step name from the calculation.
- For saturation (large IB), explicitly note why a **stiffer** (lower Rth) divider is needed.

---

## Sample 17: Acknowledging Unrealistic Results (Lab 7)

> IB = IE − IC = 1.020 mA − 1.025 mA = **−0.005 mA (−5 μA)**
>
> This small negative value for IB is physically unrealistic and is a result of minor measurement inaccuracies and component tolerances, which are amplified when subtracting two large, nearly-equal numbers. The true base current is a small positive value, as confirmed by the β ≈ 100 datasheet value, which would imply IB = IC/β ≈ 1.025 mA / 100 ≈ 10.25 μA. Both IC and IE are extremely close to the 1.0 mA design target.

### What to notice:
- **Doesn't hide the anomaly.** States the physically unrealistic result, then immediately explains the cause.
- **Diagnoses the root cause precisely**: "amplified when subtracting two large, nearly-equal numbers" — numerical precision error.
- **Provides the corrected interpretation** using an independent method (datasheet β).
- **Pivots to what the data does confirm**: "Both IC and IE are extremely close to the 1.0 mA design target."

---

## Sample 18: Junction Voltage Analysis — Confirming Operating Region (Lab 7)

> **Part 2 (Saturation Mode)**
>
> Measured Values:
> VBE = VB − VE = 2.521 V − 1.839 V = **0.682 V**
> VCE = VC − VE = 2.054 V − 1.839 V = **0.215 V**
>
> Comparison: The measured VCE of **0.215 V** is an excellent match for the pre-lab target of **0.2 V**. The measured VBE of **0.682 V** differs from the 0.8 V assumption, but this is expected as 0.8 V is a conservative design rule. The key condition for saturation is that both junctions are forward-biased. This is confirmed, as VBE > 0 and VBC = VB − VC = 2.521 V − 2.054 V = 0.467 V > 0.

### What to notice:
- **States the key condition** ("both junctions are forward-biased") before verifying it — reader knows what to look for.
- **Calculates VBC explicitly** to confirm the second junction condition, even when not directly measured.
- **Acknowledges the assumption vs. reality gap** ("0.8 V is a conservative design rule") without treating it as an error.
- **"This is confirmed, as..."** — assertive conclusion, not "this suggests" or "this may indicate."

---

## Sample 19: booktabs Table Formatting — The Right Way (Lab 8)

```latex
\begin{table}[H]
    \centering
    \caption{Part 1: Comparison of Calculated, Simulated, and Measured Values}
    \label{tab:part1_compare}
    \sisetup{round-mode=places,round-precision=3}
    \begin{tabular}{lccc}
        \toprule
        \textbf{Parameter} & \textbf{Hand Calc (Goal)} & \textbf{Simulation} & \textbf{Measured} \\
        \midrule
        $V_B$    & \SI{0}{\volt}             & \SI{1.03}{\volt}    & \SI{1.09}{\volt}   \\
        $V_E$    & \SI{0.7}{\volt}           & \SI{1.68}{\volt}    & \SI{1.755}{\volt}  \\
        $I_C$    & \SI{1.0}{\milli\ampere}   & \num{0.936e-3} A    & \num{0.898e-3} A   \\
        $\beta$  & 100 (Assumed)             & $\approx 213$       & $\approx 225$      \\
        \bottomrule
    \end{tabular}
\end{table}
```

### What to notice:
- **`\sisetup{round-mode=places,round-precision=3}`** before the tabular — consistent sig figs.
- **Column alignment**: `lccc` — left for Parameter, centered for all data columns. No `|` separators.
- **`\toprule` / `\midrule` / `\bottomrule`** — never `\hline` in a booktabs table.
- **`\textbf{}` on all column headers.**
- **Caption goes above the table** (`\caption` before `\begin{tabular}`).
- **Label format**: `tab:partN_descriptivename`.

---

## Writing Patterns Summary — LaTeX-Specific

| Task | LaTeX Command |
|------|--------------|
| All values with units | `\SI{10.83}{k\Omega}`, `\SI{1}{\milli\ampere}` |
| Approx values | `\num{0.936e-3} A` or `$\approx \SI{213}{}$` |
| Table header | `\textbf{Parameter}` in each column |
| Table rules | `\toprule` / `\midrule` / `\bottomrule` — never `\hline` |
| Table sig figs | `\sisetup{round-mode=places,round-precision=3}` |
| Design step labels | `\textbf{1. Calculating $R_E$:}` |
| Theory subsections | `\paragraph{DC Analysis}` (named inline head) |
| Discussion sub-items | `\paragraph{Comparison: X vs. Y}` |
| Bold key result | `\textbf{-196.8 \text{ V/V}}` inline or in table |
| Measured ≈ calculated | "This confirms the theory that $R_o \approx R_C$" |
| Saturation confirmed | "This is confirmed, as VBE > 0 and VBC = ... > 0" |
