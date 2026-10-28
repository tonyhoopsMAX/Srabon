(function () {
  "use strict";

  const input = document.getElementById("roman-input");
  const output = document.getElementById("bangla-output");
  const list = document.getElementById("candidate-list");
  const count = document.getElementById("candidate-count");
  const selectionLabel = document.getElementById("selection-label");
  const sampleButton = document.getElementById("sample-button");
  let selected = 0;
  let currentCandidates = [];

  function render() {
    currentCandidates = window.SrabonConverter.candidates(input.value, 6);
    selected = Math.min(selected, currentCandidates.length - 1);
    output.textContent = currentCandidates[selected] || "";
    selectionLabel.textContent = `CANDIDATE ${selected + 1}`;
    count.textContent = `${currentCandidates.length} ${currentCandidates.length === 1 ? "candidate" : "candidates"}`;
    list.replaceChildren();

    if (!input.value.length) {
      const empty = document.createElement("div");
      empty.className = "empty-state";
      empty.textContent = "Type Roman text to see transliteration candidates.";
      list.appendChild(empty);
      return;
    }

    currentCandidates.forEach((candidate, index) => {
      const button = document.createElement("button");
      button.type = "button";
      button.className = "candidate-item";
      button.setAttribute("aria-pressed", String(index === selected));
      button.setAttribute("aria-label", `Candidate ${index + 1}: ${candidate}`);

      const number = document.createElement("span");
      number.className = "candidate-index";
      number.textContent = String(index + 1);
      const text = document.createElement("span");
      text.className = "candidate-text";
      text.lang = "bn";
      text.textContent = candidate;

      button.append(number, text);
      button.addEventListener("click", () => {
        selected = index;
        render();
        const selectedButton = list.querySelector(`[aria-pressed="true"]`);
        if (selectedButton) selectedButton.focus();
      });
      list.appendChild(button);
    });
  }

  input.addEventListener("input", () => {
    selected = 0;
    render();
  });

  list.addEventListener("keydown", (event) => {
    if (event.key !== "ArrowLeft" && event.key !== "ArrowRight" && event.key !== "Home" && event.key !== "End") return;
    const items = Array.from(list.querySelectorAll("button[aria-pressed]"));
    if (!items.length) return;
    const active = items.indexOf(document.activeElement);
    if (active < 0) return;

    event.preventDefault();
    if (event.key === "Home") selected = 0;
    else if (event.key === "End") selected = currentCandidates.length - 1;
    else if (event.key === "ArrowRight") selected = (selected + 1) % currentCandidates.length;
    else selected = (selected - 1 + currentCandidates.length) % currentCandidates.length;
    render();
    const selectedButton = list.querySelector(`[aria-pressed="true"]`);
    if (selectedButton) selectedButton.focus();
  });

  sampleButton.addEventListener("click", () => {
    input.value = "ami banglay likhi";
    selected = 0;
    render();
    input.focus();
  });

  render();
})();
