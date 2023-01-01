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
        transform: props.show ? undefined : "translateX(-200%)",
      }}
    >
      {props.children}
    </div>
  );
}
