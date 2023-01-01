import React from "react";

interface DisTitleProps {
  children?: React.ReactNode;
}

export function DisTitle(props: DisTitleProps): JSX.Element {
  return <h1 className={"a2distitle"}>{props.children}</h1>;
}
