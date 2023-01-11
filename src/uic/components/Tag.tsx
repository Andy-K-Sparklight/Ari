import React from "react";

interface TagProps {
  s?: string;
}

export function Tag(props: TagProps): JSX.Element {
  if (props.s) {
    return (
      <div className={"a2tag"}>
        <br />
        {props.s.split("\n").map((s, i) => {
          if (s) {
            return (
              <React.Fragment key={i}>
                {s}
                <br />
              </React.Fragment>
            );
          }
        })}
      </div>
    );
  } else {
    return <></>;
  }
}
