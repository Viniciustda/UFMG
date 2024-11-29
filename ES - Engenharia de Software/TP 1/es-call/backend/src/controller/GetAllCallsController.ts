import HttpRestController from "../infra/http/HttpRestController";
import Request from "../infra/http/Request";
import Response from "../infra/http/Response";
import CallService from "../service/CallService";

export default class GetAllCallsController implements HttpRestController {

    private callService: CallService = new CallService();

    async execute(request: Request, response: Response): Promise<void> {
        const calls = await this.callService.getAll();
        response.status(200).send(calls);
    }
}
