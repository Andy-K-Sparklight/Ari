import React from "react";
import { useTr } from "./TP";

interface OpPanelProps {
  children?: React.ReactNode;
  show?: boolean;
  onSearch?: (s: string) => void;
}

export function OpPanel(props: OpPanelProps): JSX.Element {
  return (
    <div
      className={"a2ops"}
      style={{
        transform: props.show ? undefined : "translateX(200%)",
      }}
    >
      <div className={"a2opsin"}>
        {props.children} <br />
        <br />
      </div>
    </div>
  );
}
