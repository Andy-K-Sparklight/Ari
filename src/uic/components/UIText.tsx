import React from "react";

interface UITextProps {
  children?: React.ReactNode;
}

export function UIText(props: UITextProps): JSX.Element {
  return <div className={"a2text"}>{props.children}</div>;
}
