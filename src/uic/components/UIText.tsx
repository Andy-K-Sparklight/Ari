import React from "react";

interface UITextProps {
  text?: string;
}

export function UIText(props: UITextProps): JSX.Element {
  const lines = (props.text || "").split("\n");
  return (
    <>
      {lines.map((t, i) => {
        return (
          <div
            key={i}
            className={"a2text"}
            dangerouslySetInnerHTML={{ __html: t + "<br/>" }}
          />
        );
      })}
    </>
  );
}
