# INSTRUCTIONS.md — Global CMake Build & Library Restructuring

## 1. Objective

Analyze and restructure the existing libraries within this project to establish a unified, efficient, and maintainable library architecture.

The primary goal is to introduce a **global CMake build system** that collects the existing library components and bundles them into a single static archive (`.a`).

Additionally:
- Review and improve the internal structure of each existing library.
- Identify missing or incomplete functionality.
- Improve code organization, consistency, and maintainability.
- Create clean and well-defined public include headers for each library.
- Optimize the implementation for performance, memory efficiency, and minimal overhead.
- Preserve existing functionality and avoid unnecessary changes.

**Important:** Do not introduce new functionality without explicitly asking for approval first.

---

## 2. Initial Analysis (Mandatory)

Before making any modifications, thoroughly inspect the existing project.

Identify:

- All existing libraries and their responsibilities.
- Current source files and directory structures.
- Existing CMake configurations and build dependencies.
- Dependencies between individual libraries.
- Public and private interfaces.
- Existing header files and their usage.
- Compilation flags and platform-specific configurations.
- Existing tests, examples, and documentation.

Understand how the components interact before changing their structure.

### Deliverable

Provide a brief architectural overview containing:

1. List of existing libraries.
2. Purpose and responsibilities of each library.
3. Dependencies between libraries.
4. Identified structural issues.
5. Potential performance improvements.
6. Proposed directory and build structure.

Do not begin major restructuring until the existing architecture is understood.

---

## 3. Global CMake Build System

Create a central `CMakeLists.txt` in the project root.

This file must coordinate the compilation of all existing libraries and combine their object files into one static archive.

### Requirements

- Use a centralized build configuration.
- Avoid duplicating compiler and linker configurations across libraries.
- Define a consistent C++ standard based on the existing project requirements.
- Use modern CMake practices and target-based configuration.
- Correctly manage dependencies between library components.
- Ensure that all required source files are included.
- Generate a single static `.a` archive containing the compiled library components.
- Support clean builds and incremental builds.
- Avoid unnecessary recompilation.
- Keep the configuration extensible for future library additions.

### Static Library Creation

Use a suitable CMake architecture, such as:

- Individual object libraries for existing components.
- A global static library that collects their object files.

Example conceptual structure:

```cmake
add_library(component_a OBJECT
    ...
)

add_library(component_b OBJECT
    ...
)

add_library(global_library STATIC
    $<TARGET_OBJECTS:component_a>
    $<TARGET_OBJECTS:component_b>
)
```

This is an architectural example, not a requirement to use this exact implementation.

Ensure that:
- Object libraries receive the correct compilation settings.
- Include directories are configured consistently.
- Required dependencies are propagated correctly.
- The resulting archive contains the intended object files.
- The final library can be linked by external applications.

Do not blindly combine libraries if doing so introduces duplicate symbols, conflicting dependencies, or incompatible compilation settings.

### Build Configuration

Provide appropriate CMake options where useful, such as:

- Debug and Release configurations.
- Position-independent code when required.
- Compiler warning configurations.
- Optional testing support.
- Installation and export rules if appropriate.

Avoid unnecessary configuration options that do not serve an actual project requirement.

---

## 4. Restructuring Existing Libraries

Review every existing library individually.

Improve its internal organization while preserving its original responsibilities and behavior.

### Requirements

- Separate implementation files from public interface files.
- Group related functionality logically.
- Remove unnecessary structural complexity.
- Improve naming consistency.
- Eliminate redundant code where safe.
- Reduce unnecessary dependencies.
- Improve encapsulation.
- Ensure that each library has a clearly defined responsibility.
- Avoid circular dependencies.
- Keep internal implementation details private.

Do not merge unrelated functionality simply to reduce the number of files.

Do not change established behavior unless explicitly approved.

### Functionality Review

For each library, inspect whether its intended functionality is complete and correctly implemented.

Identify:
- Missing functions.
- Incomplete implementations.
- Unimplemented interfaces.
- Inefficient algorithms.
- Redundant operations.
- Potential resource-management issues.
- Missing error handling.
- Inconsistent API behavior.

**Approval requirement:**

If additional functionality appears necessary:

1. Explain what is missing.
2. Describe why it would be useful or necessary.
3. Explain the proposed implementation.
4. Identify potential API or compatibility changes.
5. Ask for approval.

Do not implement the proposed functionality until approval is received.

Bug fixes and improvements that preserve the existing intended behavior may be implemented without treating them as new features.

---

## 5. Public Include Headers

Create a clean public include structure for each library.

The public headers must expose the intended API without exposing unnecessary implementation details.

### Requirements

- Separate public headers from private/internal headers.
- Provide consistent header naming.
- Use include guards or `#pragma once`.
- Avoid unnecessary includes.
- Prefer forward declarations where appropriate.
- Keep interfaces minimal.
- Avoid exposing internal implementation types unless necessary.
- Ensure headers are self-contained.
- Ensure public headers compile independently.
- Avoid circular header dependencies.

### Suggested Structure

```text
project/
│
├── CMakeLists.txt
│
├── include/
│   ├── library_a/
│   │   ├── library_a.hpp
│   │   └── ...
│   │
│   ├── library_b/
│   │   ├── library_b.hpp
│   │   └── ...
│   │
│   └── global_library.hpp
│
├── src/
│   ├── library_a/
│   │   ├── ...
│   │   └── internal/
│   │
│   ├── library_b/
│   │   ├── ...
│   │   └── internal/
│
├── tests/
│
└── build/
```

Adapt this structure to the existing project instead of forcing an unnecessary complete rewrite.

### Public API

Each library should have a clearly defined entry point.

For example:

```cpp
#include <library_a/library_a.hpp>
#include <library_b/library_b.hpp>
```

The global header should provide convenient access to the intended public interfaces:

```cpp
#pragma once

#include <library_a/library_a.hpp>
#include <library_b/library_b.hpp>
```

Do not expose private implementation headers through the global header.

---

## 6. Performance and Efficiency

Performance must be considered throughout the restructuring process.

Avoid introducing unnecessary abstractions, allocations, copies, dependencies, or runtime overhead.

### Code-Level Optimization

Review:
- Unnecessary memory allocations.
- Unnecessary object copies.
- Avoidable dynamic allocations.
- Redundant computations.
- Inefficient loops and algorithms.
- Unnecessary synchronization.
- Resource ownership and lifetime management.
- Cache-unfriendly data structures.
- Excessive header dependencies and compilation overhead.

Apply appropriate C++ practices:

- Prefer move semantics where beneficial.
- Use references appropriately.
- Use `const` correctness.
- Avoid unnecessary heap allocations.
- Prefer RAII for resource management.
- Use efficient data structures.
- Minimize unnecessary template instantiations.
- Keep interfaces lightweight.

Do not optimize blindly.

Prioritize measurable improvements and preserve readability.

### Build-Level Optimization

Ensure:
- Incremental builds are efficient.
- Compilation dependencies are minimal.
- Header dependencies are controlled.
- Unnecessary source recompilation is avoided.
- Release builds use appropriate optimization settings.
- Debug builds retain useful debugging information.

Do not apply aggressive compiler flags globally if they could break compatibility or debugging.

---

## 7. Compatibility and Preservation

Existing functionality must remain intact unless explicitly approved otherwise.

Preserve:
- Existing public APIs where possible.
- Existing behavior.
- Important dependencies.
- Platform compatibility.
- Existing integration points.
- Existing tests and examples.

If API changes are unavoidable due to structural improvements, explain the reason and obtain approval before applying breaking changes.

Do not remove existing functions simply because they appear unused without verifying their purpose and external usage.

---

## 8. Testing and Validation

After implementation, verify the complete project.

### Required Checks

1. Configure the project using the root CMake configuration.
2. Build all library components.
3. Verify that the final `.a` archive is generated.
4. Inspect the archive to confirm that the expected object files are present.
5. Compile a small test application using only the public headers and generated archive.
6. Verify that all intended public interfaces are accessible.
7. Check for unresolved symbols and duplicate symbol definitions.
8. Run existing tests.
9. Verify Debug and Release builds where supported.
10. Check for compiler warnings and errors.

Example external linking test:

```bash
g++ test.cpp \
    -I./include \
    -L./build \
    -lglobal_library \
    -o test
```

Adapt the command to the actual compiler, archive name, dependencies, and platform.

Do not consider the task complete merely because CMake successfully generates the archive. The archive must also be linkable and usable.

---

## 9. Documentation

Update or create documentation describing:

- The purpose of each library.
- The public interfaces.
- The project directory structure.
- Dependencies between libraries.
- How to build the project.
- How to link the generated archive.
- How to include the public headers.
- Available build configurations.

Provide clear build instructions.

Example:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Document the actual generated archive name and location.

---

## 10. Implementation Workflow

Follow this sequence:

### Phase 1 — Inspection
- Analyze the existing project.
- Identify all libraries and dependencies.
- Understand the current architecture.
- Identify issues and improvement opportunities.

### Phase 2 — Proposal
- Present the proposed directory structure.
- Present the global CMake architecture.
- Explain the planned library restructuring.
- Identify proposed functionality additions.
- Request approval for new functionality and breaking API changes.

### Phase 3 — Build System
- Implement the root CMake configuration.
- Integrate all existing libraries.
- Configure the unified static archive.
- Verify the initial build.

### Phase 4 — Library Restructuring
- Restructure individual library components.
- Separate public and private headers.
- Improve internal organization.
- Preserve existing behavior.

### Phase 5 — Optimization
- Review performance-critical implementations.
- Remove unnecessary overhead.
- Improve memory and resource management.
- Reduce unnecessary compilation dependencies.

### Phase 6 — Validation
- Build the complete project.
- Inspect the generated archive.
- Compile an external test application.
- Run existing tests.
- Resolve integration issues.

### Phase 7 — Documentation
- Document the final structure.
- Document public interfaces.
- Document build and linking instructions.

---

## 11. Strict Rules

The following rules must be followed throughout the entire process:

1. **Do not add new functionality without asking for approval.**
2. Do not remove existing functionality without approval.
3. Do not introduce unnecessary dependencies.
4. Do not perform a complete rewrite when incremental restructuring is sufficient.
5. Do not sacrifice performance for unnecessary abstraction.
6. Do not expose internal implementation details through public headers.
7. Do not duplicate functionality across libraries.
8. Do not introduce circular dependencies.
9. Do not leave incomplete CMake configurations.
10. Do not consider the task complete until the resulting `.a` archive has been successfully built and linked by a test application.
11. Do not silently change public APIs or existing behavior.
12. Keep the implementation clean, efficient, modular, and maintainable.

---

## 12. Final Deliverables

At completion, provide:

- [ ] Global root `CMakeLists.txt`.
- [ ] Unified static `.a` archive.
- [ ] Updated individual library structures.
- [ ] Clean public include headers for every library.
- [ ] Private/internal headers separated appropriately.
- [ ] Correct dependency configuration.
- [ ] Preserved existing functionality.
- [ ] Approved functionality additions, if any.
- [ ] Performance and efficiency improvements.
- [ ] Successful Debug and Release builds where supported.
- [ ] Successful external linking test.
- [ ] Updated documentation.

### Final Report

Summarize:

1. What was changed.
2. How the directory structure changed.
3. How the global CMake system works.
4. Which libraries are included in the archive.
5. Which public headers are available.
6. What performance improvements were made.
7. Which tests were performed and their results.
8. Any remaining issues or recommendations.

**Final objective:** Deliver a cohesive, modular, performant, and maintainable library ecosystem that can be built through one global CMake configuration and distributed as a single static `.a` archive, with clean public headers and clearly separated internal implementations.