# CONTRIBUTING

## Merging

- Please make sure to pull remote commits using **rebase** if working on the same branch (like `main`).
  
  - `git pull --rebase` will always do the trick.
  
- For Pull Requests, the following rules apply:
  
  - **Use squash** if the PR only contains 1-2 commits.
  
  - **Use merge commit** if the PR contains more than 2 commits.

  - **Avoid using rebase** as it won't keep signature.

## Commits

- Make sure to sign your commits using GPG key (or any other way you like) so that we can make sure it's you who are commiting.

- If a commit is not signed, we can still merge it, but we will ask you first to confirm.

- Please write the commit message in the format of:
  
  ```
  oper(scope): show description
  
  - Long Description
  - Long Description
  
  Fixed #1
  CHECKED
  ```
  
  Where:
  
  - `oper`: `feat`, `fix`, `chore`, `style`, `remove` or `docs`.
  
  - `scope`: Any scope you like. Also be aware of special scopes like `ci`, `*`, `sys`, etc.
  
  - Do not use the upper case of the capital letter in short description, scope and oper. Use the lower case will be better. However, use it in the long description.
  
  - `Fixed #1`: Or `Resolved #1` if you **NEED TO CLOSE THE ISSUE**, otherwise use `Might be helpful with #1` or `Working on #1` will be better.
    
    If the commit is not related to any issue, do not use this line.
  
  - `CHECKED`: Can be:
    
    - `CHECKED`: If you've verified it works.
    
    - `UNCHECKED`: It should be tested but you haven't or not able to.
    
    - `BUGS KNOWN`: Known to have bugs, but still needs to commit.
    
    - Nothing: If the code does not need to be tested.

- If your PR does not have a valid commit message, the commits might be squashed and a new message will be assigned by us. It's painful for both you and us, so avoid doing that!

## Testing

- Do self tests before commiting. We believe you but we will also review the code in the PR.

- Do not write stupid bugs that can be easily find out.

## Code Style

- If you are using `clang-format`, it's recommended to use the following config:
  
  ```
  { BasedOnStyle: GNU, SortIncludes: false, SpaceBeforeParens: Never }
  ```

- If you do not use a formatter, or do not like the style above, feel free to use your own style for commiting. We'll format the code base regularly.
  
  - You could also do a format before commiting and we will appreciate that.
  
  - If only the last commit is formatted (i.e. the final version), please consider using **squash** when merging.

## Code Guidelines

We don't want to limit your creativity, so there are no strict rules, but it's better to consider the following suggestions:

### Backend (C/C++)

- Avoid including new libaries unless it's really necessary.

- Output debug log at necessary points.

- Avoid writing platform related code unless no cross version is available.

- Do not block the main thread or the uv thread.

- Do not build wheels if they already exist in the repo.

### Frontend (TypeScript JSX etc.)

- Avoid creating huge components.

- Avoid doing any network request in the page.

- Limit the number of calls to the backend.

- Consider the compatibility of Webview before using modern browser features.
