const path = require("path");

module.exports = {
  entry: {
    Main: "./src/uic/App.tsx",
    ModrinthTweak: "./src/uic/ModrinthTweak.ts",
  },
  output: {
    filename: "[name].js",
    path: path.resolve(__dirname, "dist"),
    pathinfo: false,
  },
  module: {
    rules: [
      {
        test: /\.tsx?$/,
        include: path.resolve(__dirname, "src"),
        use: {
          loader: "ts-loader",
          options: {
            transpileOnly: true,
            experimentalWatchApi: true,
          },
        },
        exclude: /node_modules/,
      },
      {
        test: /\.css$/i,
        use: ["style-loader", "css-loader"],
      },
      {
        test: /\.ya?ml$/,
        use: "yaml-loader",
      },
    ],
  },
  resolve: {
    extensions: [".ts", ".js", ".tsx", ".css", ".yaml"],
  },
  devtool: "eval-source-map",
  mode: "development",
};
