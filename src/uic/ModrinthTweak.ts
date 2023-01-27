setInterval(() => {
  if (document.getElementById("ach")) {
    return;
  }
  const modregex = /^\/mod\/[0-9A-Za-z\-]+$/i;
  document.querySelector(".user-controls > .auth-prompt")?.remove();
  document.querySelector(".modrinth-content-wrapper")?.remove();
  if (modregex.test(window.location.pathname)) {
    const ele = document.createElement("div");
    ele.classList.add("markdown-body", "card");
    ele.style.backgroundColor = "var(--color-brand)";
    ele.style.cursor = "pointer";
    ele.style.color = "var(--color-brand-inverted)";
    ele.style.display = "table";
    ele.style.width = "95%";
    ele.onmouseenter = () => {
      ele.style.opacity = "90%";
    };
    ele.onmouseleave = () => {
      ele.style.opacity = "100%";
    };
    ele.onclick = () => {
      if (navigator.language.includes("zh-CN")) {
        btnText.innerHTML = "请稍等...";
      } else {
        btnText.innerHTML = "Please Wait...";
      }

      btnText.style.color = "var(--color-text)";
      ele.style.backgroundColor = "var(--color-raised-bg)";
      document
        .querySelectorAll("div.value.lowercase > button.code > svg")
        .forEach((e) => {
          e.remove();
        });
      const pjID = document
        .querySelector("div.value.lowercase > button.code")
        ?.innerHTML.trim();
      if (pjID) {
        (async () => {
          const res = await fetch("http://localhost:46432/add", {
            method: "POST",
            body: pjID,
          });
          console.log(res.status);
          if (res.ok) {
            if (navigator.language.includes("zh-CN")) {
              btnText.innerHTML = "模组已安装!";
            } else {
              btnText.innerHTML = "Mod added!";
            }
          } else {
            ele.style.backgroundColor = "red";
            btnText.style.color = "inherit";
            if (navigator.language.includes("zh-CN")) {
              btnText.innerHTML = "模组安装失败, 请检查启动器.";
            } else {
              btnText.innerHTML = "Failed to add mod!";
            }
          }
        })();
      }
    };
    ele.id = "ach";

    const btnText = document.createElement("p");
    btnText.style.display = "table-cell";
    btnText.style.fontSize = "1.4em";
    btnText.style.width = "100%";
    btnText.style.height = "100%";

    btnText.style.textAlign = "center";
    if (navigator.language.includes("zh-CN")) {
      btnText.innerHTML = "\uff0b 安装模组";
    } else {
      btnText.innerHTML = "\uff0b Add To Ari";
    }
    ele.appendChild(btnText);
    document
      .querySelector(".navigation-card")
      ?.insertAdjacentElement("beforebegin", ele);
  }
}, 100);

export {};
