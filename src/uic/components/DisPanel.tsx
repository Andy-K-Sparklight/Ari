import React from "react";

interface DisPanelProps {
  children?: React.ReactNode;
  show?: boolean;
}

export function DisPanel(props: DisPanelProps): JSX.Element {
  return (
    <div
      className={"a2dispanel"}
      style={{
        opacity: props.show ? undefined : "0",
      }}
    >
      {props.children}
    </div>
  );
}
