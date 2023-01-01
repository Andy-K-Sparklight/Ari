import React, { useEffect, useState } from "react";

interface MaskProps {
  color?: string;
}

export function Mask(props: MaskProps): JSX.Element {
  const [innerShow, setInnerShow] = useState(false);
  useEffect(() => {
    window.addEventListener("+A2WarnMask", () => {
      setInnerShow(true);
    });
    window.addEventListener("-A2WarnMask", () => {
      setInnerShow(false);
    });
  }, []);
  const color = props.color || "#00000000";
  const dis = innerShow ? undefined : "none";
  return (
    <>
      <div
        className={"a2mask"}
        style={{
          top: 0,
          left: 0,
          right: 0,
          height: "2rem",
          display: dis,
          background: "linear-gradient(to bottom, " + color + ", #00000000)",
        }}
      ></div>
      <div
        className={"a2mask"}
        style={{
          left: 0,
          top: 0,
          bottom: 0,
          width: "2rem",
          display: dis,
          background: "linear-gradient(to right, " + color + ", #00000000)",
        }}
      ></div>
      <div
        className={"a2mask"}
        style={{
          bottom: 0,
          left: 0,
          right: 0,
          height: "2rem",
          display: dis,
          background: "linear-gradient(to top, " + color + ", #00000000)",
        }}
      ></div>
      <div
        className={"a2mask"}
        style={{
          right: 0,
          top: 0,
          bottom: 0,
          width: "2rem",
          display: dis,
          background: "linear-gradient(to left, " + color + ", #00000000)",
        }}
      ></div>
    </>
  );
}
