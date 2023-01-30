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
  const confirmProg = useRef(0);
  const [confirmProgDisplay, setProg] = useState(0);
  const mouseDown = useRef(false);
  const interval = useRef(-1);

  const progress = () => {
    if (props.multiSelect) {
      return;
    }
    if (mouseDown.current) {
      confirmProg.current += 2;
      setProg(confirmProg.current);
      if (confirmProg.current >= 100) {
        confirmProg.current = 0;
        if (props.onClick) {
          mouseDown.current = false;
          props.onClick();
        }
      }
    } else {
      confirmProg.current = 0;
      setProg(confirmProg.current);
    }
  };

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
          mouseDown.current = true;
          interval.current = setInterval(progress, 10) as unknown as number;
        }}
        onClick={() => {
          if (props.multiSelect && props.onClick) {
            props.onClick();
          }
          clearInterval(interval.current);
          setProg(0);
        }}
        onMouseUp={() => {
          mouseDown.current = false;
          clearInterval(interval.current);
          setProg(0);
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
        <div
          style={{
            position: "absolute",
            backgroundColor: "#89898950",
            width: confirmProgDisplay + 5 + "%",
            height: "100%",
            bottom: 0,
            left: "-5%",
            overflow: "hidden",
          }}
        ></div>
      </div>
    </>
  );
}
