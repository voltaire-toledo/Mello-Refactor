# GitHub Copilot Workflow

## Development Philosophy

This project uses a **spec-driven development** approach powered by GitHub Copilot.

## Workflow Steps

### 1. Feature Specification
- Write detailed specifications in `docs/specs/`
- Include requirements, acceptance criteria, and test cases
- Use Copilot Chat to refine specifications

### 2. Architecture Planning
- Discuss architectural decisions with Copilot
- Document in `docs/architecture/`
- Create diagrams and flowcharts

### 3. Implementation
- Use Copilot for code generation
- Leverage Copilot Chat for complex logic
- Iterate with Copilot on improvements

### 4. Testing
- Generate unit tests with Copilot
- Use Copilot for test case suggestions
- Verify edge cases

### 5. Documentation
- Auto-generate documentation with Copilot
- Keep inline comments minimal but meaningful
- Update README and specs

## Copilot Best Practices

### 1. Context Management
- Keep related files open in editor
- Reference existing code patterns
- Use descriptive variable names

### 2. Prompt Engineering
- Be specific about requirements
- Provide examples of desired output
- Ask for explanations when needed

### 3. Code Review
- Always review generated code
- Understand before accepting
- Refactor for clarity

### 4. Incremental Development
- Small, focused changes
- Test frequently
- Commit often with clear messages

## Example Prompts

### Architecture Discussion
```
"Design a configuration loading system that supports both JSON and YAML, 
with hot-reload capabilities and validation. Consider PowerToys' approach."
```

### Implementation
```
"Implement a Windows low-level keyboard hook using WH_KEYBOARD_LL that 
detects modifier keys (Win, Ctrl, Shift, Alt) and virtual key codes. 
Handle cleanup properly and prevent memory leaks."
```

### Testing
```
"Generate unit tests for the ConfigurationLoader class, including edge cases
for malformed JSON, missing files, and invalid hotkey combinations."
```

## Learning Journal

Keep notes in `docs/learning/` about:
- New Windows APIs discovered
- C++ patterns learned
- Copilot interactions that were particularly helpful
- Challenges and solutions