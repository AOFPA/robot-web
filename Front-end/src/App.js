import axios from "axios";
import React from "react";
// Bootstrap CSS
import "bootstrap/dist/css/bootstrap.min.css";
// Bootstrap Bundle JS
import "bootstrap/dist/js/bootstrap.bundle.min";

const baseURL = "http://127.0.0.1:3000/post";

export default function App() {
  const [post, setPost] = React.useState(null);
  const [order1, setOrder1] = React.useState(null);
  const [order2, setOrder2] = React.useState(null);

  React.useEffect(() => {
    axios.get(`${baseURL}/1`).then((response) => {
      setPost(response.data);
    });
  }, []);

  function createPost() {
    axios
      .post(baseURL, {
        order1: order1,
        order2: order2
      })
      .then((response) => {
        setPost(response.data);
      });
  }

  // if (!post) return "No post!"

  return (
    <div className="container text-center mt-5">
      <div className="card">
        <div className="card-header bg-primary text-light">OMA FORM</div>
        <div className="card-boddy p-3">
          <div className="form-group">
            <label>order1</label>
            <input className="form-control" type="text" value={order1} onChange={(e) => setOrder1(e.target.value)} />
          </div>
          <div className="form-group mt-2">
            <label>order2</label>
            <input className="form-control" type="text" value={order2} onChange={(e) => setOrder2(e.target.value)} />
          </div>
          <button className="btn btn-primary mt-2" onClick={createPost}>Send</button>
        </div>
      </div>

    </div>

  );
}