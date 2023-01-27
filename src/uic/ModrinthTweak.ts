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
    ele.id = "ach";

    const btnText = document.createElement("p");
    btnText.style.display = "table-cell";
    btnText.style.fontSize = "1.4em";
    btnText.style.width = "100%";
    btnText.style.height = "100%";

    btnText.style.textAlign = "center";
    btnText.innerHTML = "\uff0b Add To Launcher";
    ele.appendChild(btnText);
    document
      .querySelector(".navigation-card")
      ?.insertAdjacentElement("beforebegin", ele);
  }
}, 100);

export {};
