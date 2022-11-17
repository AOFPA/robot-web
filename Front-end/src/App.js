import axios from "axios";
import React, { useEffect, useState } from "react";

// Bootstrap CSS
import "bootstrap/dist/css/bootstrap.min.css";
// Bootstrap Bundle JS
// import "bootstrap/dist/js/bootstrap.bundle.min";

const baseURL = "http://127.0.0.1:3000";

export default function App() {
  const [post, setPost] = React.useState(null);
  const [order1, setOrder1] = React.useState(null);
  const [order2, setOrder2] = React.useState(null);
  const [dir, setDir] = React.useState(null);

  const [mode, setMode] = React.useState(null);

  //STATUS
  const [status, setStatus] = useState({});

  //css
  const [css, setCss] = useState(null);

  let i = 0;
  let robotStatus = "";

  const getStatus = () => {
    i++;
    axios.get(`${baseURL}/status`).then((res) => {
      // console.log(res.data);
      setStatus((status) => ({
        ...status,
        ...res.data,
      }));
      console.log("from status" + i);
      console.log(status);
      if (status.robot === "not online") {
        setCss("text-danger fw-bold")
      } else {
        setCss("text-success fw-bold");
      }
    });
  };

  useEffect(() => {
    const interval = setInterval(getStatus, 5000);

    return () => clearInterval(interval);
  });

  // React.useEffect(() => {
  //   axios.get(`${baseURL}/1`).then((response) => {
  //     setPost(response.data);
  //   });
  // }, []);

  function createPost() {
    axios
      .post(`${baseURL}/post`, {
        order1: order1,
        order2: order2,
      })
      .then((response) => {
        setPost(response.data);
        console.log();
        console.log(post);
      });
  }

  //ส่ง mode
  function sendMode() {
    axios
      .post(`${baseURL}/mode`, {
        mode: mode,
      })
      .then((response) => {
        setPost(response.data);
        console.log(response);
      });
  }

  function sendDir(inp) {
    setDir(inp);
    console.log(inp);
    axios
      .post(`${baseURL}/dir`, {
        dir: inp,
      })
      .then((res) => {
        console.log(res);
      });
  }

  // if (!post) return "No post!"

  return (
    <div className="container text-center mt-5">
      <div className="row">
        <div className="col-12 col-md-6">
          <div className="card">
            <div className="card-header">Order</div>
            <div className="card-boddy p-3">
              <div className="form-group">
                <label>order1</label>
                <input
                  className="form-control"
                  type="text"
                  value={order1}
                  onChange={(e) => setOrder1(e.target.value)}
                />
              </div>
              <div className="form-group mt-2">
                <label>order2</label>
                <input
                  className="form-control"
                  type="text"
                  value={order2}
                  onChange={(e) => setOrder2(e.target.value)}
                />
              </div>
              <button className="btn btn-primary mt-2" onClick={createPost}>
                Send
              </button>
            </div>
          </div>
        </div>
        <div className="col-12 col-md-6">
          <div className="card">
            <div className="card-header">Manual Control</div>
            <div className="card-body">
              <div className="row">
                <div className="col-12 mb-2">
                  {" "}
                  <label>MODE : Auto = a, Manual = m, b=กด btn, r=reset</label>
                  <input
                    className="form-control"
                    type="text"
                    value={mode}
                    onChange={(e) => setMode(e.target.value)}
                  />
                  <div className="col-12 mt-3">
                    <button
                      className="btn btn-info"
                      onClick={() => sendMode()}
                    >
                      send mode
                    </button>
                  </div>
                </div>
                <hr />
                <div className="col-1"></div>
                <div className="col-10">
                  <button
                    className="btn btn-primary"
                    onClick={() => sendDir("forward")}
                  >
                    Forward
                  </button>
                </div>
                <div className="col-1"></div>

                <div className="col-1"></div>
                <div className="col-10 mt-3 ">
                  <button
                    className="btn btn-warning"
                    onClick={() => sendDir("left")}
                  >
                    Left
                  </button>
                  <button
                    className="btn btn-danger mx-2"
                    onClick={() => sendDir("stop")}
                  >
                    Stop
                  </button>
                  <button
                    className="btn btn-warning"
                    onClick={() => sendDir("right")}
                  >
                    Right
                  </button>
                </div>
                <div className="col-1"></div>

                <div className="col-1"></div>
                <div className="col-10 mt-3">
                  <button
                    className="btn btn-primary"
                    onClick={() => sendDir("backward")}
                  >
                    Backward
                  </button>
                </div>
                <div className="col-1"></div>
              </div>

              <h2>direction : {dir}</h2>
            </div>
          </div>
        </div>
        <div className="col-12 col-md-6">
          <div className="card">
            <div className="card-header">Robot Status</div>
            <div className="card-body">
              Robot status :{" "}
              <span className={css}>{status.robot}</span>
              <br />
              MQTT status :{" "}
              <span className={css}>{status.mqtt}</span>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
