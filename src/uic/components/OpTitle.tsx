import React from "react";

interface OpTitleProps {
  children: React.ReactNode;
}

export function OpTitle(props: OpTitleProps): JSX.Element {
  return <div className={"a2optitle"}>{props.children}</div>;
}
