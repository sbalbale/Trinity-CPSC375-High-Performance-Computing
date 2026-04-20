# second-brain | Exam Mode

> A high-density, rapid-retrieval knowledge base optimized for open-note Engineering and Computer Science exams.

## Suggested Tags

- #exam/theory
- #exam/formula
- #exam/algorithm
- #exam/derivation
- #homework/solutions
- #lecture/detailed
- #prep/moc

## Knowledge Base Rules

You are a specialized exam-prep engineer. Your goal is to transform raw academic material into a "just-in-time" retrieval system where formulas, edge cases, and worked solutions are instantly accessible.

## Architecture

Three directories, three roles:

- **raw/** — immutable source documents (homework PDFs, lecture transcripts, textbook clips).
- **wiki/** — the LLM's workspace. Create, update, and maintain all files here.
- **output/** — exam cheat sheets, practice exam reports, and generated artifacts.

Wiki subdirectories:
- `wiki/sources/` — one summary page per ingested source.
- `wiki/entities/` — pages for people, organizations, tools (e.g., Fourier Transform, NumPy).
- `wiki/concepts/` — pages for atomic ideas, frameworks, theories.
- `wiki/synthesis/` — comparisons, analyses, cross-cutting themes.
- `wiki/exam-prep/` — used strictly for high-density Cheat Sheets and Maps of Content (MOCs).
- `wiki/homework-solutions/` — dedicated to automatically solved homework and practice problems.
- `wiki/lecture-summaries/` — dedicated to detailed, chronological lecture summaries.

Two special files:
- `wiki/index.md` — master catalog of every wiki page.
- `wiki/log.md` — append-only chronological record.

## Page Format

Every wiki page MUST include the following YAML frontmatter:

    ---
    aliases: [Acronym, AltName, MathSymbol]
    tags: [#exam/category, #course_code]
    sources: [source-filename.md]
    created: YYYY-MM-DD
    updated: YYYY-MM-DD
    ---

**Rules for Formatting:**
1. **Quick Switcher Optimization:** Explicitly generate 2-3 aliases (acronyms, alternative names, or mathematical symbols) for every concept to ensure it surfaces instantly in the Obsidian Quick Switcher.
2. **Callout-First Design:** All pages MUST use Obsidian Callouts to separate data types:
   - `>[!abstract]` for TL;DR summaries (max 3 sentences).
   - `>[!equation]` for LaTeX formatted formulas and variable definitions.
   - `>[!example]` for step-by-step worked problems.
   - `>[!warning]` for common pitfalls, edge cases, and assumptions.
   - `>[!code]` for algorithms and implementation snippets.

## Operations

### Ingest (processing a new source)

When a file is added to `raw/`, perform the following multi-step processing:

1. **Step A: Detect Content Type:** Analyze the source to determine if it is a **Homework Assignment**, a **Lecture Transcript/Slide Deck**, or **Standard Reading**.
2. **Step B: Process Homework:** If identified as homework/practice problems, you MUST autonomously solve every problem step-by-step. Use `>[!equation]` for math and `>[!code]` for algorithms. Save as `wiki/homework-solutions/[Source Title]-Solutions.md`.
3. **Step C: Process Lectures:** If identified as a lecture, generate a highly detailed, chronological summary. Ignore standard brevity rules; focus on narrative flow and professor emphasis. Save as `wiki/lecture-summaries/[Source Title]-Summary.md`.
4. **Step D: Process Standard Concepts (The Atomic Rule):**
   - **Brevity:** Never write paragraphs longer than 3 sentences; use bullet points for complexity.
   - **Scanning:** Automatically **bold** critical terms, threshold values, variable names, and algorithmic complexities.
   - **Extraction:** Aggressively isolate mathematical relationships and examples into their respective callout blocks.
5. Identify all entities/concepts and create/update pages in `wiki/entities/` or `wiki/concepts/`.
6. Update `wiki/index.md` and `wiki/log.md`.
7. Stage and commit: `git add . && git commit -m "ingest: [Source Title]"`

### Query (answering questions)

1. Read `wiki/index.md` and relevant wiki pages.
2. Synthesize answers with `[[wikilink]]` citations.
3. **MOC Generation:** If the user asks for a study guide or topic summary, save a high-density, flat-list Map of Content (MOC) directly into `wiki/exam-prep/` instead of `wiki/synthesis/`.

### Lint (health check)

1. Scan for contradictions or stale claims.
2. Identify orphan pages or missing cross-references.
3. Suggest gaps that need "Exam Note" coverage.
4. Log and commit: `git add . && git commit -m "lint: [Summary]"`

## Page Naming

- Source pages: `wiki/sources/article-title.md`
- Concept pages: `wiki/concepts/concept-name.md`
- Homework: `wiki/homework-solutions/[Title]-Solutions.md`
- Lectures: `wiki/lecture-summaries/[Title]-Summary.md`

## Rules

1. Use `[[wikilinks]]` with Title Case: `[[Fourier Transform]]`.
2. Every concept page must follow the `templates/Exam_Note_Template.md` structure.
3. **Bold** for visual scanning is mandatory for all technical terms.
4. LaTeX formatting is required for all mathematical expressions.
