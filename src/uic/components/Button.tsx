import React from "react";
import { IMAGES } from "./ImgSrc";
import { Tag } from "./Tag";

interface ButtonProps {
  onClick?: () => void;
  text?: string;
  hint?: string;
  warn?: string;
  img?: string;
  tag?: string;
  compact?: boolean;
}

export function Button(props: ButtonProps): JSX.Element {
  let hintContent = "";
  let cname = "a2btn";
  switch (props.hint) {
    case "warn":
      hintContent = props.warn || "";
      break;
    case "opt":
      hintContent = "★";
      cname += " a2opt";
      break;
  }
  if (props.compact) {
    cname += " a2btncompact";
  }

  return (
    <>
      <Tag s={props.tag} />
      <div
        style={{
          borderColor:
            props.hint == "warn" ? "var(--a2-warn)" : "var(--a2-base)",
        }}
        onClick={() => {
          if (props.onClick) {
            props.onClick();
          }
          if (props.hint == "warn") {
            window.dispatchEvent(new CustomEvent("-A2WarnMask"));
          }
        }}
        className={cname}
        onMouseEnter={() => {
          if (props.hint == "warn") {
            window.dispatchEvent(new CustomEvent("+A2WarnMask"));
          }
        }}
        onMouseLeave={() => {
          if (props.hint == "warn") {
            window.dispatchEvent(new CustomEvent("-A2WarnMask"));
          }
        }}
      >
        <div className={"a2img"}>{IMAGES[props.img || ""]}</div>
        <div>
          {" " + (props.text || "") + " "}
          <span className={"a2hint" + (props.hint || "")}>{hintContent}</span>
        </div>
      </div>
    </>
  );
}
