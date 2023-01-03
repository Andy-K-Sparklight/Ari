import React from "react";
import { OpTitle } from "./OpTitle";
import { useTr } from "./TP";

interface OpPanelProps {
  children?: React.ReactNode;
  show?: boolean;
  onSearch?: (s: string) => void;
}

export function OpPanel(props: OpPanelProps): JSX.Element {
  const [ti, updateTi] = useTr("Op.Title");
  return (
    <div
      className={"a2ops"}
      style={{
        transform: props.show ? undefined : "translateX(200%)",
      }}
      onClick={() => {
        updateTi();
      }}
    >
      <div className={"a2opsin"}>
        <OpTitle>{ti}</OpTitle>
        <hr />
        <div className={"a2opsbtns"}>
          {props.children} <br />
          <br />
        </div>
      </div>
    </div>
  );
}
