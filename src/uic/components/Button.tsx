import React, { ReactNode } from "react";
import { IMAGES } from "./ImgSrc";

interface ButtonProps {
  children?: ReactNode;
  onClick?: () => void;
  text?: string;
  hint?: "warn" | "opt";
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
      {props.tag ? <div className={"a2tag"}>{props.tag}</div> : ""}
      <div
        style={{
          borderColor:
            props.hint == "warn" ? "var(--a2-warn)" : "var(--a2-base)",
        }}
        onClick={props.onClick}
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
        <img className={"a2img"} src={IMAGES[props.img || ""]}></img>
        {" " + (props.text || "") + " "}
        <span className={"a2hint" + (props.hint || "")}>{hintContent}</span>
      </div>
    </>
  );
}
