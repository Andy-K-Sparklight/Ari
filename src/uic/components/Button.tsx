import React, { useEffect, useRef, useState } from "react";
import { IMAGES } from "./ImgSrc";
import { Tag } from "./Tag";

interface ButtonProps {
  onClick?: () => void;
  text?: string;
  hint?: string;
  warn?: string;
  sub?: string;
  img?: string;
  tag?: string;
  compact?: boolean;
  multiSelect?: boolean; // Multiple select mode
  selected?: boolean;
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
  if (props.multiSelect && props.selected) {
    cname += " hv";
  }

  return (
    <>
      <Tag s={props.tag} />
      <div
        style={{
          borderColor:
            props.hint == "warn" ? "var(--a2-warn)" : "var(--a2-base)",
          overflow: "hidden",
        }}
        onMouseDown={() => {
          if (props.onClick) {
            props.onClick();
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
        <div className={"a2img"}>
          {(() => {
            const k = props.img || "";
            if (IMAGES[k] !== undefined) {
              return IMAGES[k];
            } else {
              return <img src={"data:image;base64," + k} />;
            }
          })()}
        </div>
        <div>
          {" " + (props.text || "") + " "}
          <span className={"a2hint" + (props.hint || "")}>{hintContent}</span>
          <span className={"a2sub" + (props.hint || "")}>
            {props.sub || ""}
          </span>
        </div>
      </div>
    </>
  );
}
