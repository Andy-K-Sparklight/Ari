# CONTRIBUTING

## Principles

- Solve problems first.

- No verbal attacks.

- Do as the Romans do.

## Basic Rules

- All issues and pull requests **MUST be written in English**. Avoid using translations unless you can make sure they won't cause any misunderstanding.

- Follow the collaborators' standards. e.g. You believe you've found a bug but others don't agree so, then it's better not to argue more about this.

- Let closed things get closed. All issues and PRs will be locked after some days of inactivity (3 days for issues and 14 days for PRs unless they have label `help wanted`).

- Don't duplicate issue or PR. If the previous one was closed as `resolved` or `not planned`, then let the past be past. However, DO open a new one if the problem re-appeared.

- Don't be off-topic. Instead, open a new one otherwise things will get "partially solved", which is complicated and cumbersome.

- You agree your code, if commited, will be licensed under *GNU General Public License*, Version 3 or later.

## Open an issue or PR

### When To

- You've tested the application, and you've found a bug with no proper solution.
  
  - **PR!** If you know how to solve this, open a pull request instead an issue. Even if you're not sure whether it's the correct way. Collaborators will review the changes and test them.
  
  - **Issue!** If not, please at least provide some guesses. The more information you provide, the easier we're able to get things done.

- You've got a feature request.
  
  - **PR!** Still, please open a PR if you know how to implement.
  
  - **Issue!** If not, by providing some ideas will also speed up things.

- You've found a security problem.
  
  - **Issue!** If it's not severe, simply open it as a bug to be solved.
  
  - **DO NOT!** If the problem is severe, **DO NOT** open an issue. Instead, report using GitHub's *Private vulnerability reporting* to report them. E-mail collaborators are also acceptable, but not guaranteed to be answered.

- You've got some ways to improve the code.
  
  - **PR!** Always open a pull request in these cases. Criticizing existing code without suggesting proper improvements will be treated as vexation.

### How to create an issue correctly

#### Bugs

- What is the problem, including:
  
  - What does it behave like.
  
  - How to reproduce.

- Environment, including:
  
  - OS variant and version (`uname -a`).
  
  - Hardware information (Mostly CPU and RAM).
  
  - Application branch and version (At the left corner of the window).
  
  - Network performance.

- Suggestions of how to resolve or resonable guesses.

- Additional information.

#### Feature requests

- What is the feature, including:
  
  - How it should be like, in detail.
  
  - How necessary it is.
  
  - Why do you want this feature, or why existing features can't cover your requirement.

- Ideas of how to implement it.

#### Security related

- What is the problem, including:
  
  - Affected code.
  
  - Application branch and version.
  
  - How severe this problem is.

- Ideas of how to fix it.

### How to create a PR correctly

- In your PR there should be following content:
  
  - What does this PR solve.
  
  - Does this PR contains any flaws unresolved.

- Description for changes are not necessarily required, collaborators will review your changes by diffs.

- After you've created a PR, please **watch it**, make sure to catch the latest updates. Your PR will be automatically verified using CI specified.
  
  - If all checks have passed, congratulations!
  
  - If not, please check what's the problem and resolve it.
    
    - Any PR that fails to pass the automatic checks **won't** be merged, even collaborators can't help you.
    
    - If you believe that's a CI bug, please open an issue to fix it first.

- Collaborators will review your changes. Please contact with them and resolve their suggestions.

- After all conditions have been matched, your PR will be merged automatically. They are:
  
  - At least 1 approving review from collaborators.
  
  - All required checks have passed.

- If the PR has been inactive for 14 days it will be closed and locked. You should contact one of the collaborators to require reviewing for your PR if we failed to do that in a while.

## Code standards

We're always trying to let nothing but your imagination to limit your creativity. However, there are still some rules that need to be followed.

The following list is not completed and might be changed at any time. Only the code commited after changes needs to follow them, of course.

- **Rules** are rules, they **SHOULE NEVER** be violated, at any time.

- **Guidelines** are relatively looser, they should be followed unless under certain (and very few) circumstances.

#### Rules

- All C/C++ and JavaScript/TypeScript code MUST be formatted.
  
  For C/C++ code, `.clang-format` is like:
  
  ```yml
  BasedOnStyle: GNU
  SortIncludes: false
  SpaceBeforeParens: Never
  Standard: Auto
  ```
  
  For JavaScript/TypeScript code, use `prettier` formatter and use its default configuration.
  
  The only exception is the vendored library code.

- For C/C++, `goto` and `asm` are both **STRICTLY FORBIDDEN**.

- For C++, `malloc` and `free` are forbidden. Unless being used to free memory allocated by a C library.

- For C/C++, only `//` is allowed to start a comment. For JavaScript/TypeScript, `//` and `/* */` are both acceptable. Leave a space between `//` and the first letter.

- Blocking the main thread for long time tasks is forbidden. In most cases, the main thread shouldn't be responsible for any background tasks at all.

- All code must be GCC compatible. Clang is not strictly required, but please try before giving up.

- Loading urls in webview using user provided values are strictly forbidden.

#### Guidelines

- All non-locale related code should be able to be encoded using ASCII only. Which means no special characters are allowed.

- Prefer using existing API over implementing a new one. e.g. use built-in `DownloadPack` instead of your own download method to make downloads trackable and faster.

- Avoid using platform (C/C++) or browser (JavaScript/TypeScript) dependent APIs. Create a crossing one for all platforms if you must to.

## Join Us

If you want more access to the codebase and development resources, become a collaborator will be a good choice! To do that, send an E-mail to the repository owner. We hope to meet you!
